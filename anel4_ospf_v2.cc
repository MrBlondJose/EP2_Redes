/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>
#include <string>
#include <cassert>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/nstime.h"
#include "ns3/output-stream-wrapper.h"

#include <chrono> 
#include <bits/stdc++.h> 

using namespace std; 
using namespace std::chrono; 

using namespace ns3;

/**
 * \class StackHelper
 * \brief Helper to set or get some IPv6 information about nodes.
 */
class StackHelper
{
public:

  /**
   * \brief Add an address to a IPv6 node.
   * \param n node
   * \param interface interface index
   * \param address IPv6 address to add
   */
  inline void AddAddress (Ptr<Node>& n, uint32_t interface, Ipv6Address address)
  {
    Ptr<Ipv6> ipv6 = n->GetObject<Ipv6> ();
    ipv6->AddAddress (interface, address);
  }

  /**
   * \brief Print the routing table.
   * \param n the node
   */
  inline void PrintRoutingTable (Ptr<Node>& n)
  {
    Ptr<Ipv6StaticRouting> routing = 0;
    Ipv6StaticRoutingHelper routingHelper;
    Ptr<Ipv6> ipv6 = n->GetObject<Ipv6> ();
    uint32_t nbRoutes = 0;
    Ipv6RoutingTableEntry route;

    routing = routingHelper.GetStaticRouting (ipv6);

    std::cout << "Routing table of " << n << " : " << std::endl;
    std::cout << "Destination\t\t\t\t" << "Gateway\t\t\t\t\t" << "Interface\t" <<  "Prefix to use" << std::endl;

    nbRoutes = routing->GetNRoutes ();
    for (uint32_t i = 0; i < nbRoutes; i++)
      {
        route = routing->GetRoute (i);
        std::cout << route.GetDest () << "\t"
                  << route.GetGateway () << "\t"
                  << route.GetInterface () << "\t"
                  << route.GetPrefixToUse () << "\t"
                  << std::endl;
      }
  }
};

int main (int argc, char** argv)
{
    NodeContainer c;
    c.Create(4);

    NodeContainer n0n1 = NodeContainer( c.Get(0), c.Get(1) );
    NodeContainer n1n2 = NodeContainer( c.Get(1), c.Get(2) );
    NodeContainer n2n3 = NodeContainer( c.Get(2), c.Get(3) );
    NodeContainer n3n0 = NodeContainer( c.Get(3), c.Get(0) );

    //Iniciando links sem ip
    //NS_LOG_INFO ("Create channels.");
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

    NetDeviceContainer d0d1 = p2p.Install( n0n1 );
    NetDeviceContainer d1d2 = p2p.Install( n1n2 );
    NetDeviceContainer d2d3 = p2p.Install( n2n3 );
    NetDeviceContainer d3d0 = p2p.Install( n3n0 );

    //Instalando pilhas de protocolos/aplicacoes nos nos
    InternetStackHelper internet;
    internet.Install (c); 
    
    //ipv4
    //NS_LOG_INFO ("Assign IP Addresses.");
    Ipv6AddressHelper ipv6;

    ipv6.SetBase (Ipv6Address ("2001:1::"), Ipv6Prefix (64));
    Ipv6InterfaceContainer ic1 = ipv6.Assign (d0d1);

    ipv6.SetBase (Ipv6Address ("2001:2::"), Ipv6Prefix (64));
    Ipv6InterfaceContainer ic2 = ipv6.Assign (d1d2);

    ipv6.SetBase (Ipv6Address ("2001:3::"), Ipv6Prefix (64));
    Ipv6InterfaceContainer ic3 = ipv6.Assign (d2d3);

    ipv6.SetBase (Ipv6Address ("2001:4::"), Ipv6Prefix (64));
    Ipv6InterfaceContainer ic4 = ipv6.Assign (d3d0);

    auto start = high_resolution_clock::now(); 
    //Criando tabelas de roteamento via ospf
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    auto stop = high_resolution_clock::now();
	auto duration = duration_cast<nanoseconds>(stop - start);
	cout << duration.count() << " nanoseconds" << endl;
	
    //Print de tabela de roteamento para no 0
	
	
     
}
