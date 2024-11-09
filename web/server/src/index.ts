import { ServiceRegistry } from "./config/service-registry";
import { ExpressService } from "./service/express-service";
import { MqttService } from "./service/mqtt-service";

const serviceRegistry: ServiceRegistry = new ServiceRegistry();
const mqttService: MqttService = new MqttService();

void serviceRegistry
  .registerService(mqttService)
  .registerService(new ExpressService(mqttService))
  .init();

process.on("SIGINT", handleTermination);
process.on("SIGTERM", handleTermination);

function handleTermination(args) {
  console.info(`Received ${args} - shutting down`);
  serviceRegistry.destructServices().then(() => process.exit(0));
}
