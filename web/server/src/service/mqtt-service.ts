import { Service } from "../model/service";
import mqtt from "mqtt";

export class MqttService implements Service {
  public readonly name = "MQTT";
  public readonly environmentVariables = ["MQTT_HOST", "MQTT_PORT"];

  private client: mqtt.MqttClient;

  public init(): Promise<void> {
    const host = process.env.MQTT_HOST;
    const port = process.env.MQTT_PORT;

    return new Promise((resolve) => {
      this.client = mqtt.connect(`mqtt://${host}:${port}`);
      this.client.on("connect", (_packet) => {
        console.log(`Connected to MQTT server at ${host}:${port}`);
        resolve();
      });
    });
  }

  public publish(topic: string, data: string): Promise<void> {
    return new Promise((resolve, reject) =>
      this.client.publish(topic, data, {}, (error?: Error) => {
        if (error != null) {
          reject(error);
        } else {
          resolve();
        }
      }),
    );
  }

  public destruct(): Promise<void> {
    return new Promise((resolve) => {
      this.client.end();
      this.client.on("disconnect", (_packet) => resolve());
    });
  }
}
