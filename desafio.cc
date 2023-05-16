#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

// Default Network Topology
//
//       10.1.1.0
// nA -------------- nB -------------- nC
//    point-to-point
//

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

int main(int argc, char *argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);
    // Isto ira fazer com que as aplicações mostrem as mensagens dos pacotes
    // sendo enviados e recebidos durante a simulação.
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    // cria três nós
    NodeContainer nodes;
    nodes.Create(3);

    // NetDevice: placa de rede // Channel: cabo
    PointToPointHelper pointToPoint;
    // 5Mbps de taxa de transmissão
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    // 2ms de atraso
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer devicesAB, devicesBC; // declara o container de dispositivos de rede
    // para cada no em NodeContainer um PointToPointNetDevice é criado e adicionado ao
    // NetDeviceContainer
    devicesAB = pointToPoint.Install(nodes.Get(0), nodes.Get(1));
    devicesBC = pointToPoint.Install(nodes.Get(1), nodes.Get(2));
    /*Depois de executar a chamada pointToPoint.Install (nodes) iremos ter dois nós, cada qual
    instalado na rede ponto-a-ponto e um único canal ponto-a-ponto ligando os dois. Ambos os
    dispositivos serão configurados para ter uma taxa de transferência de dados de cinco megabits
    por segundo, que por sua vez tem um atraso de transmissão de dois milissegundos.*/

    /*O InternetStackHelper é um assistente de topologia inter-rede. O método Install utiliza um
    NodeContainer como parâmetro. Quando isto é executado, ele irá instalar a pilha de protocolos da
    Internet (TCP, UDP, IP, etc) em cada nó do contêiner.*/
    InternetStackHelper stack;
    stack.Install(nodes);

    // atribui enderecos IP
    Ipv4AddressHelper address1, address2;
    address1.SetBase("10.1.1.0", "255.255.255.0");
    address2.SetBase("10.2.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces1 = address1.Assign(devicesAB);
    Ipv4InterfaceContainer interfaces2 = address2.Assign(devicesBC);

    UdpEchoServerHelper echoServer(9);

    /*É este método que instancia a aplicação de servidor de eco (echo server) e a associa ao nó*/
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(2));

    // Inicia a aplicação do servidor em 1 segundo e para em 10 segundos
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 9);

    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    // Inicia a aplicação do cliente em 2 segundos e para em 10 segundos
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    Simulator::Run();
    Simulator::Destroy();

    /*O sistema verifica se os arquivos foram construídos e então executa-os. Através do componente
    de registro vemos que o cliente enviou 1024 bytes para o servidor através do IP 10.1.1.2. Também
    podemos ver que o servidor diz ter recebido 1024 bytes do IP 10.1.1.1 e ecoa o pacote para o
    cliente, que registra o seu recebimento.*/
    return 0;
}