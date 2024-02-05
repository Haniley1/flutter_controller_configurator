import 'package:shared_preferences/shared_preferences.dart';

class DeviceStorage {
  static const ALL_DEVICES_KEY = "DEVICES_UUID_LIST";

  void addDevice(String uuid, String title) async {
    final prefs = await SharedPreferences.getInstance();

    List<String> savedDevices = prefs.getStringList(ALL_DEVICES_KEY) ?? [];
    savedDevices.add(uuid);

    prefs.setString(uuid, title);
    prefs.setStringList(ALL_DEVICES_KEY, savedDevices);
  }

  void removeDevice(String uuid) async {
    final prefs = await SharedPreferences.getInstance();

    prefs.remove(uuid);
  }

  Future<String?> getDeviceName(String uuid) async {
    final prefs = await SharedPreferences.getInstance();

    return prefs.getString(uuid);
  }

  Future<Map<String, String>> getSavedDevices() async {
    final prefs = await SharedPreferences.getInstance();
    final Map<String, String> result = {};
    final List<String> savedDevices = prefs.getStringList(ALL_DEVICES_KEY) ?? [];

    for (String uuid in savedDevices) {
      final name = await getDeviceName(uuid);
      if (name == null || name.isEmpty) {
        continue;
      }

      result[uuid] = name!;
    }

    return result;
  }
}
