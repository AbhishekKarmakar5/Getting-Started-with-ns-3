/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/netanim-module.h"

// Default Network Topology
//
//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -------------- n1   n2   n3   n4
//                   point-to-point  |    |    |    |
//                                   ================
//                                     LAN 10.1.2.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nCsma = 3;
  uint32_t nWifi = 3;
  bool tracing = false;

  CommandLine cmd;
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);

  // The underlying restriction of 18 is due to the grid position
  // allocator's configuration; the grid layout will exceed the
  // bounding box if more than 18 nodes are provided.
  if (nWifi > 18)
    {
      std::cout << "nWifi should be 18 or less; otherwise grid layout exceeds the bounding box" << std::endl;
      return 1;
    }

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (1));
  csmaNodes.Create (nCsma);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);

                        //Wireless

  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nWifi);
  NodeContainer wifiApNode = p2pNodes.Get (0); // Access Point Node

/* we need to set up 3 things -> Channel, physical layer, MAC- medium access control*/

  // YANS -> Yet Another Network Simulator
  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();// we are using default channel location or default channel properties such as frequency modulation. The whole coding behind this is done by an Electronics student or telecom student.
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ()); // Creating a communication path


// we browse internet through wifiManager or a router
  WifiHelper wifi; // setting up a wifi for a particular node
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");// AarfWifiManager is one of SetRemoteStationManager

  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns-3-ssid");       // ssid is the broadcast id. when we switch on our wifi on phone , the names of wifi signals are the ssid.
// what kind of mac has been saved
  mac.SetType ("ns3::StaWifiMac",               // StationWifiMac
               "Ssid", SsidValue (ssid),        // 
               "ActiveProbing", BooleanValue (false)); // two types of nodes in wifi -> station nodes(moving nodes) & access point nodes (fixed due to ethernet connectivity)
        // ActiveProbing keeps on checking whether there is any wifi or not in the nearby area.

  NetDeviceContainer staDevices;
  staDevices = wifi.Install (phy, mac, wifiStaNodes);

// AP another model of wifi
  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer apDevices;
  apDevices = wifi.Install (phy, mac, wifiApNode);

  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",          // nodes are positioned on grid system
                                 "MinX", DoubleValue (0.0),             // minimum x
                                 "MinY", DoubleValue (0.0),             // minimum y
                                 "DeltaX", DoubleValue (5.0),           // difference in x
                                 "DeltaY", DoubleValue (10.0),          // difference in y
                                 "GridWidth", UintegerValue (3),        // width in grid
                                 "LayoutType", StringValue ("RowFirst"));// RowFirst is the LayoutType

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel", // z axis is missing
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility.Install (wifiStaNodes); // moving model

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel"); // the nodes are position is fixed and no RectangleValue has to be mentioned.
  mobility.Install (wifiApNode);

  InternetStackHelper stack;
  stack.Install (csmaNodes);
  stack.Install (wifiApNode);
  stack.Install (wifiStaNodes);

  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  address.Assign (staDevices);
  address.Assign (apDevices);

  UdpEchoServerHelper echoServer (9);

//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -----------------n1   n2   n3   n4 (SERVER)
//            ^       point-to-point  |    |    |    |
//            |                       ================
//         Client                       LAN 10.1.2.0

  ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (nCsma));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (nCsma), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (wifiStaNodes.Get (nWifi - 1));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables (); // ns3 provides routing tables algorithm to make GlobalRoutingTable

  AnimationInterface anim ("mythird.xml"); // Generate animation file
  anim.SetConstantPosition(csmaNodes.Get(0),10.0,10.0);
  anim.SetConstantPosition(csmaNodes.Get(1),10.0,30.0);
  anim.SetConstantPosition(csmaNodes.Get(2),10.0,50.0);
  anim.SetConstantPosition(csmaNodes.Get(3),10.0,70.0);

  anim.SetConstantPosition(wifiStaNodes.Get(1),30.0,10.0);
  anim.SetConstantPosition(wifiStaNodes.Get(2),50.0,10.0);
  anim.SetConstantPosition(wifiStaNodes.Get(0),70.0,10.0);

  anim.SetConstantPosition(wifiApNode.Get(0),100.0,100.0);      
  Simulator::Stop (Seconds (10.0));


 /* if(tracing==true)
  {
         AsciiTraceHelper ascii;
         pointToPoint.EnableAsciiAll (ascii.CreateFileStream("mythird1.tr"));
         phy.EnableAscii (ascii.CreateFileStream ("mythird2.tr"), apDevices.Get (0));
         csma.EnableAscii (ascii.CreateFileStream ("mythird3/tr"), csmaDevices.Get (0));
  }*/

  if (tracing == true)
    {
      pointToPoint.EnablePcapAll ("p2p");
      phy.EnablePcap ("wireless", apDevices.Get (0));   // for wifi
      csma.EnablePcap ("csma", csmaDevices.Get (0), true); // for only one node
    }

 


  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
