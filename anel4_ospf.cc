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

int main (int argc, char** argv)
{
    //Criando conjunto de nos
    NodeContainer c;
    c.Create(4);
    //Agrupando nos para instalacao de dispositivos e canal de rede
    NodeContainer n0n1 = NodeContainer( c.Get(0), c.Get(1) );
    NodeContainer n1n2 = NodeContainer( c.Get(1), c.Get(2) );
    NodeContainer n2n3 = NodeContainer( c.Get(2), c.Get(3) );
    NodeContainer n3n0 = NodeContainer( c.Get(3), c.Get(0) );

    //Iniciando links sem ip
    PointToPointHelper p2p;
    //Padroes do enlace
    p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
    //Criando sub-redes
    NetDeviceContainer d0d1 = p2p.Install( n0n1 );
    NetDeviceContainer d1d2 = p2p.Install( n1n2 );
    NetDeviceContainer d2d3 = p2p.Install( n2n3 );
    NetDeviceContainer d3d0 = p2p.Install( n3n0 );

    Ipv4GlobalRoutingHelper routingHelper;
    //Instalando pilhas de protocolos/aplicacoes nos nos
    InternetStackHelper internet;
    internet.Install (c); 
    
    //Adicionando enderecos ipv4
    Ipv4AddressHelper ipv4;

    ipv4.SetBase ("10.0.0.0", "255.255.255.0");
    Ipv4InterfaceContainer ii01 = ipv4.Assign(d0d1);

    ipv4.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer ii12 = ipv4.Assign(d1d2);

    ipv4.SetBase ("10.2.2.0", "255.255.255.0");
    Ipv4InterfaceContainer ii23 = ipv4.Assign(d2d3);

    ipv4.SetBase ("10.3.3.0", "255.255.255.0");
    Ipv4InterfaceContainer ii30 = ipv4.Assign(d3d0);

     
    //Criando tabelas de roteamento via ospf
    routingHelper.PopulateRoutingTables();
          
    //Criando estruturas para print de tabelas de roteamento    
    Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> (&std::cout);

    Ipv4ListRoutingHelper::PrintRoutingTableAllAt (Seconds (1.0), routingStream);	

    Simulator::Stop (Seconds (10.0));
    auto start = high_resolution_clock::now();
    Simulator::Run ();
    auto stop = high_resolution_clock::now();
	auto duration = duration_cast<nanoseconds>(stop - start);
	cout << duration.count() << " nanoseconds" << endl;

    Simulator::Destroy ();    
}
