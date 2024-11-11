const HOST = ".";

export function publishToTopic(topic: string, data: object): Promise<void> {
  const requestOptions = {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(data),
  };

  return fetch(`${HOST}/publish/${topic}`, requestOptions)
    .then((response) => response.json())
    .then((data) => console.log(data))
    .catch((error) => console.error("There was an error!", error));
}
