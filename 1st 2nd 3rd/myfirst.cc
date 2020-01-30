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
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  // Input from the command Line -> ./waf --run scratch/myfirst --b=25
  // Argument variables are passed in argc and argv[]

  Time::SetResolution (Time::NS); // not actually require, by default the speed of the network has been set to NanoSec, we can set it to sec
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
  // The ns-3 logging facility can be used to monitor or debug the progress of simulation programs. 
  // Logging output can be enabled by program statements in your main() program or by the use of the NS_LOG environment variable.


                                        // MAIN PART OF THE CODE
  NodeContainer nodes;
  nodes.Create (2); // In the stack of Ns-3, two nodes are being created.       A  &  B

  PointToPointHelper pointToPoint;      // Link from point to point     ----------      only the link is created
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

//                      5Mbps
//      A       ----------------------  B
//                      2Ms             

  NetDeviceContainer devices;   // for connecting the two nodes together
  devices = pointToPoint.Install (nodes);
//                5Mbps
//      A----------------------B
//                 2Ms             

  InternetStackHelper stack; // we need an IP address to communicate to between the two nodes for this we need InternetStackHelper
  stack.Install (nodes); // install the nodes on the stack, this stack is different from the ns-3 stack

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
// using Ipv4AddressHelper to set the Base and by default mask
//                 5Mbps
//      A----------------------B        ( Domain to the network = 10.1.1.0)
//                 2Ms  
// whenever the IP address is declared, the 1st node is 10.1.1.1 and 2nd node is 10.1.1.2

  Ipv4InterfaceContainer interfaces = address.Assign (devices); // we now need to interface the devices using the two IP addresses



// Now we need to run an application that run from A to B or from B to A to say that the application has been append between A and B    
//In order to do so, we install the application on the nodes -> one as server, other as client   
  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1)); // Application has been installed using echoServer
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

//                5Mbps
//      A----------------------B
//                 2Ms             
//      Client                 Server

  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);        // taking the address of device1 i.e B
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

//                5Mbps
//      A----------------------B
//                 2Ms             
//      Client --------------> Server
//              Its Mentioned Echo
//      Server <------------- Client

        
  AnimationInterface anim ("myfirst.xml");
  anim.SetConstantPosition (nodes.Get(0), 1.0,1.0);
  anim.SetConstantPosition (nodes.Get(1), 100.0,100.0);

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
