import { Service } from "../model/service";
import express, { Request, Response } from "express";
import cors from "cors";
import { MqttService } from "./mqtt-service";
import { Server } from "http";

export class ExpressService implements Service {
  public readonly name = "Express";
  public readonly environmentVariables = ["PORT"];

  constructor(private readonly mqttService: MqttService) {}

  private server: Server;

  public init(): Promise<void> {
    const app = express();
    const port = process.env.PORT;

    app.use(express.static("app"));
    app.use(express.json());
    app.use(cors());

    app.post("/publish/:topic", this.publish.bind(this));

    return new Promise(
      (resolve) =>
        (this.server = app.listen(port, () => {
          console.log(`Listening on port ${port}`);
          resolve();
        })),
    );
  }

  private publish(req: Request, res: Response) {
    const topic = req.params.topic;
    const body = req.body;
    const bodyString = JSON.stringify(body);

    console.log("Received topic body", topic, bodyString);

    this.mqttService.publish(topic, bodyString).then((err) => {
      if (err) {
        console.log("Error when publishing", err);
        res.json({ success: false, error: err });
      } else {
        console.log("Published", err);
        res.json({ success: true });
      }
    });
  }

  public destruct(): Promise<void> {
    return new Promise((resolve) => this.server.close(() => resolve()));
  }
}
