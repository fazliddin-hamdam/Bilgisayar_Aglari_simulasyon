#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lorawan-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("LoRaWANSimulation");

static void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize, uint32_t pktCount, Time pktInterval) {
    if (pktCount > 0) {
        socket->Send (Create<Packet> (pktSize));
        Simulator::Schedule (pktInterval, &GenerateTraffic, socket, pktSize, pktCount - 1, pktInterval);
    } else {
        socket->Close ();
    }
}

int main (int argc, char *argv[]) {
    CommandLine cmd;
    cmd.Parse (argc, argv);

    // Log level ayarları
    LogComponentEnable ("LoRaWANSimulation", LOG_LEVEL_INFO);

    // Node'ları oluştur
    NodeContainer endDevices;
    endDevices.Create (10);
    NodeContainer gateways;
    gateways.Create (1);

    // Mobilite ayarları
    MobilityHelper mobility;
    mobility.SetPositionAllocator ("ns3::RandomRectanglePositionAllocator",
                                   "X", StringValue ("ns3::UniformRandomVariable[Min=-50.0|Max=50.0]"),
                                   "Y", StringValue ("ns3::UniformRandomVariable[Min=-50.0|Max=50.0]"));
    mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                               "Bounds", RectangleValue (Rectangle (-100, 100, -100, 100)));
    mobility.Install (endDevices);

    // LoRaWAN ağ kurulumu
    LoraHelper lora;
    lora.Install (endDevices);
    lora.Install (gateways);

    // Internet yığını kurulumu
    InternetStackHelper internet;
    internet.Install (gateways);
    internet.Install (endDevices);

    // Uygulama katmanı
    uint16_t port = 9;
    Address localAddress (InetSocketAddress (Ipv4Address::GetAny (), port));
    PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", localAddress);
    ApplicationContainer sinkApp = packetSinkHelper.Install (gateways.Get (0));
    sinkApp.Start (Seconds (0.0));
    sinkApp.Stop (Seconds (20.0));

    // Trafik yaratma
    OnOffHelper onOffHelper ("ns3::UdpSocketFactory", Address ());
    onOffHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    onOffHelper.SetAttribute ("PacketSize", UintegerValue (1024));
    onOffHelper.SetAttribute ("DataRate", StringValue ("14kbps")); // LoRa tipik data hızı
    ApplicationContainer clientApps = onOffHelper.Install (endDevices.Get (0));
    clientApps.Start (Seconds (1.0));
    clientApps.Stop (Seconds (19.0));

    // Simülasyonu başlat
    Simulator::Stop (Seconds (20.0));
    Simulator::Run ();
    Simulator::Destroy ();

    return 0;
}
