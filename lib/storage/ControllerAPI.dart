import 'package:http/http.dart' as http;

class ControllerAPI {
  Future<int> getTemp(String address) async {
    final response = await http.get(Uri.parse("$address/temp/get-current"));
    return int.parse(response.body);
  }
}
