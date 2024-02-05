import 'package:flutter/material.dart';
import 'package:flutter_controller_configurator/pages/DeviceForm.dart';
import 'package:upnp2/upnp.dart';

class SSPDListView extends StatelessWidget {
  const SSPDListView({
    super.key,
    required this.devices,
  });

  final List<Device> devices;

  @override
  Widget build(BuildContext context) {
    void openDeviceForm(Device selectedDevice) {
      Navigator.push(context, MaterialPageRoute(builder: (context) => DeviceForm(ssdpDevice: selectedDevice)));
    }

    if (devices.isEmpty) {
      return const Padding(padding: EdgeInsets.all(8), child: Row(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          Text("Нет данных", style: TextStyle(fontSize: 16, fontWeight: FontWeight.w500),),
        ],
      ),);
    }

    return Padding(
      padding: const EdgeInsets.all(8.0),
      child: ListView.separated(
        separatorBuilder: (BuildContext context, int index) => const Divider(
          height: 7.0,
          color: Colors.black45
        ),
        itemCount: devices.length,
        itemBuilder: (BuildContext context, int index) {
          return SizedBox(
            child: GestureDetector(
              onTap: () => openDeviceForm(devices[index]),
              child: Column(
                mainAxisAlignment: MainAxisAlignment.start,
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text("Модель: ${devices[index].modelName!}"),
                  Text("Наименование: ${devices[index].friendlyName!}"),
                  Text("Адрес: ${devices[index].url}")
                ],
              )
            ),
          );
        }
      ),
    );
  }
}