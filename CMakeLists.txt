belirtin
set(NS3_INSTALL_DIR "/home/bilmuh/ns-allinone-3.41/ns-3.41/sumo-1.10.0")

# NS-3 başlık dosyalarını ve kütüphanelerini ekleyin
include_directories(${NS3_INSTALL_DIR}/include)
link_directories(${NS3_INSTALL_DIR}/lib)

# Proje dosyalarını ekleyin
add_executable(sumo-1.10.0 lorawanprojesi.cc)

# Bağımlılıkları belirtin
target_link_libraries(sumo-1.10.0
    ns3.41-core
    ns3.41-buildings
    ns3.41-energy
    ns3.41-mobility
    ns3.41-point-to-point
    ns3.41-stats
    ns3.41-lorawan
    ns3.41-network
    ns3.41-propagation
)
