#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"
using namespace ns3;

int
main (int argc, char *argv[])
{
	//creating the network nodes
	NodeContainer hosts;
	NodeContainer routers;
	NodeContainer branch;

	hosts.Create(1);
	routers.Create(4);
	branch.Create(3);

	// For installing internet stack on each node
	InternetStackHelper stack;
	stack.Install(hosts);
	stack.Install(routers);
	stack.Install(branch);

	//Create CSMA helper
	CsmaHelper csma;

	//Create a PointToPOint Helper
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  	p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
	
	//Create an Address helper
	Ipv4AddressHelper address;
	
								// SUBNET 1


	//Create a node container to hold the nodes that belong to subnet 1 and add workstation 1 and router 1
	NodeContainer subnet1;
	subnet1.Add(hosts.Get(0));
	subnet1.Add(routers.Get(0));

	//Create a device container to hold net devices installed on each other
	NetDeviceContainer subnet1Devices = p2p.Install(subnet1);

	//Configure the subnet address and mask
	address.SetBase("10.1.1.0","255.255.255.0");

	//Create an interface container to hold the ipv4 interfaces created and assign IP address to interface
	Ipv4InterfaceContainer subnet1Interfaces = address.Assign(subnet1Devices);
	
								// SUBNET 2


	//Create a node container to hold the nodes that belong to subnet 2 and add router 1 and router 2
	NodeContainer subnet2;
	subnet2.Add(routers.Get(0));
	subnet2.Add(routers.Get(1));

	//Create a device container to hold net devices installed on each routers
	NetDeviceContainer subnet2Devices = p2p.Install(subnet2);

	//Configure the subnet address and mask
	address.SetBase("10.1.2.0","255.255.255.0");

	//Create an interface container to hold the ipv4 interfaces created and assign IP address to each interface
	Ipv4InterfaceContainer subnet2Interfaces = address.Assign(subnet2Devices);

								// SUBNET 3

	//Create a node container to hold the nodes that belong to subnet 3 and add router 2 and router 3
	NodeContainer subnet3;
	subnet3.Add(routers.Get(1));
	subnet3.Add(routers.Get(2));

	//Create a device container to hold net devices installed on each routers
	NetDeviceContainer subnet3Devices = p2p.Install(subnet3);

	//Configure the subnet address and mask
	address.SetBase("10.1.3.0","255.255.255.0");

	//Create an interface container to hold the ipv4 interfaces created and assign IP address to each interface
	Ipv4InterfaceContainer subnet3Interfaces = address.Assign(subnet3Devices);


								// SUBNET 4

	//Create a node container to hold the nodes that belong to subnet 4 and add router 2 and router 4
	NodeContainer subnet4;
	subnet4.Add(routers.Get(1));
	subnet4.Add(routers.Get(3));

	//Create a device container to hold net devices installed on each routers
	NetDeviceContainer subnet4Devices = p2p.Install(subnet4);

	//Configure the subnet address and mask
	address.SetBase("10.1.4.0","255.255.255.0");

	//Create an interface container to hold the ipv4 interfaces created and assign IP address to each interface
	Ipv4InterfaceContainer subnet4Interfaces = address.Assign(subnet4Devices);

								//SUBNET 5
	
	//Create a node container to hold the nodes that belong to subnet5 and add router 3 and the branch nodes
	NodeContainer subnet5;
	subnet5.Add(routers.Get(2));
	subnet5.Add(branch);

	//Create a device container to hold the net devices installed on each node
	NetDeviceContainer subnet5Devices = csma.Install(subnet5);

	//Configure the subnet address and mask
	address.SetBase("10.1.5.0","255.255.255.0");

	//Create an interface container to hold the ipv4 interfaces created and assign IP address to each interface
	Ipv4InterfaceContainer subnet5Interfaces = address.Assign(subnet5Devices);

	AnimationInterface anim ("tutorial4.xml");

								// Run the Simulator
	Simulator::Run();
	Simulator::Destroy();
	return 0;
}
