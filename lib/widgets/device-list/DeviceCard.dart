import 'package:flutter/material.dart';
import 'package:upnp2/upnp.dart';

import '../../pages/DeviceForm.dart';

class DeviceCard extends StatelessWidget {
  const DeviceCard({super.key, required this.device});

  final Device device;

  @override
  Widget build(BuildContext context) {
    final minTemp = 18;
    final maxTemp = 27;
    final currentTemp = 24;
    final violationTemp = currentTemp >= maxTemp || currentTemp <= minTemp;

    const tresholdTempsTextStyle =
        TextStyle(fontWeight: FontWeight.w500, color: Colors.black38);
    final currentTempTextStyle = TextStyle(
        fontSize: 18,
        fontWeight: FontWeight.w600,
        color: violationTemp ? Colors.red : Colors.black);

    void openDeviceForm() {
      Navigator.push(
          context,
          MaterialPageRoute(
              builder: (context) => DeviceForm(ssdpDevice: device)));
    }

    return Card(
        child: InkWell(
            onTap: openDeviceForm,
            child: Padding(
                padding: const EdgeInsets.all(16),
                child: Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      Container(
                          margin: const EdgeInsets.only(bottom: 16),
                          child: const Row(children: [
                            Text("Test",
                                style: TextStyle(
                                    fontSize: 24, fontWeight: FontWeight.w700))
                          ])),
                      Row(
                          mainAxisAlignment: MainAxisAlignment.spaceBetween,
                          children: [
                            Text("Мин: ${minTemp}°C",
                                style: tresholdTempsTextStyle),
                            Text("Текущая: ${currentTemp}°C",
                                style: currentTempTextStyle),
                            Text("Макс: ${maxTemp}°C",
                                style: tresholdTempsTextStyle)
                          ])
                    ]))));
  }
}
