# Oficina de IoT - Planta inteligente

Material de referência para a oficina de IoT com a planta inteligente.

## Preparação

- Baixar e instalar IDE do [Arduino](www.arduino.cc)
- Instalar driver [CP2102](http://www.silabs.com/products/mcu/pages/usbtouartbridgevcpdrivers.aspx)
- Configurar IDE do Arduino para o NodeMCU
  - Arquivo → Preferências → Configurações → "additional boards manager URLs"
    - Mudar para: http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Ferramentas → Board → Boards Manager
  - Baixar "Esp8266 by esp8266 community"
  - Ferramentas → Board → NodeMCU 1.0 (ESP-12E Module)
- Testando a conexão PC - NodeMCU
  - Arquivo → Exemplos → ESP8266 → Blink
  - Compile e carregue o programa em sua placa NodeMCU
