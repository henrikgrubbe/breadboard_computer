export interface Service {
  name: string;
  init: () => Promise<void>;
  destruct: () => Promise<void>;
  environmentVariables: string[];
}
