import 'package:flutter/material.dart';
import 'package:flutter_controller_configurator/pages/DeviceForm.dart';
import 'package:flutter_controller_configurator/storage/DeviceStorage.dart';
import 'package:flutter_controller_configurator/widgets/device-list/DeviceCard.dart';
import 'package:upnp2/upnp.dart';

class DeviceListView extends StatelessWidget {
  const DeviceListView({
    super.key,
    required this.devices,
  });

  final List<Device> devices;

  @override
  Widget build(BuildContext context) {
    final deviceStorage = DeviceStorage();

    void openDeviceForm(Device selectedDevice) async {
      final title = await deviceStorage.getDeviceName(selectedDevice.uuid!);

      if (context.mounted) {
        Navigator.push(context, MaterialPageRoute(builder: (context) => DeviceForm(ssdpDevice: selectedDevice, title: title)));
      }
    }

    return Padding(
      padding: const EdgeInsets.symmetric(horizontal: 4, vertical: 8),
      child: ListView.builder(
        itemCount: devices.length,
        itemBuilder: (BuildContext context, int index) {
          return DeviceCard(device: devices[index],);
          // return SizedBox(
          //   child: GestureDetector(
          //     onTap: () => openDeviceForm(devices[index]),
          //     child: Column(
          //       mainAxisAlignment: MainAxisAlignment.start,
          //       crossAxisAlignment: CrossAxisAlignment.start,
          //       children: [
          //         Text(devices[index].modelName!),
          //         Text(devices[index].friendlyName!),
          //       ],
          //     )
          //   ),
          // );
        }
      ),
    );
  }
}