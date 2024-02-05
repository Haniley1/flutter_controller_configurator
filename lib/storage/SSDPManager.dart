import 'package:flutter/widgets.dart';
import 'package:flutter_controller_configurator/storage/SSDPStorage.dart';
import 'package:provider/provider.dart';
import 'package:upnp2/upnp.dart';

class SSDPManager {
    const SSDPManager({ required this.onDeviceFound });

    final ValueSetter<Device> onDeviceFound;

    discoverDevices() async {
      final discover = DeviceDiscoverer();

      await discover.start(ipv6: false);
      discover.quickDiscoverClients().listen((client) async {
        try {
          final device = await client.getDevice();
          onDeviceFound(device!);
          print('Found device: ${device!.friendlyName}: ${device.url} ${device.presentationUrl}');
        } catch (e, stack) {
          print('ERROR: $e - ${client.location}');
          print(stack);
        }
      });
    }
}
