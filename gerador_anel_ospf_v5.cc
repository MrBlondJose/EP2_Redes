/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */

#include <iostream>
#include <stack>
#include <string>
#include <cassert>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/ipv6-routing-table-entry.h"
#include "ns3/ipv6-static-routing-helper.h"
#include <chrono> 
#include <bits/stdc++.h> 
#include "ns3/netanim-module.h"
#include <iostream>
#include <list>
#include <string>

using namespace std; 
using namespace std::chrono; 
using namespace ns3;

list<NodeContainer> NodeContainerStackCreator(){
 list <NodeContainer> l; 
 return l;
}

/***
Essa função constroi uma topologia anel com os containers. Faltam fazer as funções que instanciam os enlaces o configuram os ips para todos os nós.
**/
list<NodeContainer> BuildRing(NodeContainer c, int size){
  int i=0;
  list<NodeContainer> l;
  for(i=0;i<size-1;i++){
        
        NodeContainer n = NodeContainer (c.Get(i),c.Get(i+1));
        l.push_back(n);
  }
  NodeContainer n = NodeContainer (c.Get(i),c.Get(0));
  l.push_back(n);
  return l;
}

string GetByIndex(list<string> l,int index){
 int i=0;
 for(string v: l){
  if(i==index){
   return v;
  }
  i=i+1;
 }
 return NULL;
}

list<NetDeviceContainer> PointToPointCreator(list<NodeContainer> nodes,list<string> Parameters, list<string> ParametersValues,list<string> channels, list<string> channelValue){

 PointToPointHelper p2p;

 int i=0;
 for(string S: Parameters){
   string pv=GetByIndex(ParametersValues,i);
   p2p.SetDeviceAttribute(S,StringValue(pv));
   i=i+1;
 }

 i=0;
 for(string S: channels){
   string pv=GetByIndex(channelValue,i);
   p2p.SetChannelAttribute(S,StringValue(pv));
   i=i+1;
 }

 list<NetDeviceContainer> l;
 for (NodeContainer N: nodes){
  l.push_back(p2p.Install(N));
 }
 return l;
}

list<Ipv4InterfaceContainer> IPV4Assign (list<NetDeviceContainer> links){
  Ipv4AddressHelper ipv4;
  list<Ipv4InterfaceContainer> l;

  ipv4.SetBase ("10.0.0.0", "255.255.255.0");
  int i=0;
  for(NetDeviceContainer N: links){
    string ini_ip= "10."+std::to_string(i)+"."+std::to_string(i)+".0";
    
    ipv4.SetBase (ini_ip.c_str(), "255.255.255.0");
    l.push_back(ipv4.Assign(N));
    i++;
  }
  return l;
} 

void RoutingTablesCreator(){
    auto start = high_resolution_clock::now(); // Captura o tempo da máquina 
    //Criando tabelas de roteamento via ospf
    Ipv4GlobalRoutingHelper::PopulateRoutingTables(); // Popula tabelas com  OSPF

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(stop - start);
    cout << duration.count() << " nanoseconds" << endl; 
}

void RunSimulation() {
    Simulator::Stop (Seconds (10.0));
    auto start = high_resolution_clock::now();
    Simulator::Run ();
    auto stop = high_resolution_clock::now();
	auto duration = duration_cast<nanoseconds>(stop - start);
    Simulator::Destroy ();
	cout << duration.count();        
}

void CreateSimulation(int NumberOfNodes) {
    NodeContainer c;
    c.Create(NumberOfNodes);

    list<NodeContainer> l=BuildRing(c, NumberOfNodes);

    list<string> Parameters;
    list <string> channels;
    list<string> ParametersValues;
    list<string> channelValue;
    //Setando paratros para os enlaces ps2 // 
    Parameters.push_back("DataRate");
    channels.push_back("Delay");
    ParametersValues.push_back("5Mbps");
    channelValue.push_back("2ms");

    list<NetDeviceContainer> links=PointToPointCreator(l,Parameters,ParametersValues,channels,channelValue); //Funcao que cria os enlaces

    InternetStackHelper internet;
    internet.Install (c);
    IPV4Assign(links);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables(); // Popula tabelas com  OSPF
}

int main (int argc, char** argv)
{
    int N_max_nos = 100;
    int N_sims_por_cenario = 10;
    int N_nos = 0;
    for (N_nos = 2; N_nos <= N_max_nos; N_nos++) {
        //cria topologia para cada simulacao
        CreateSimulation(N_nos);
        cout << N_nos << ";";
        //rodar a simulacao 10 vezes por tamanho de topologia
        int iterador = 0;
        for(iterador = 0; iterador < N_sims_por_cenario; iterador++){
            RunSimulation();
            if(! (iterador == N_sims_por_cenario-1)) cout << ";";
        }   
        cout << endl;        
    }
}
