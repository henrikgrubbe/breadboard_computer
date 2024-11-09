# Stage 1: Compile and build server
FROM node:22-alpine AS server-build

WORKDIR /usr/local/server
COPY ./server /usr/local/server

RUN npm ci --quiet
RUN npm run build


# Stage 2: Compile and build app
FROM node:22-alpine AS app-build

WORKDIR /usr/local/app
COPY ./app /usr/local/app

RUN npm ci --quiet
RUN npm run build


# Stage 3: Run server
FROM node:22-alpine

WORKDIR /server
COPY --from=server-build /usr/local/server/dist .
COPY --from=app-build /usr/local/app/dist ./app
COPY .server/package*.json ./

ENV NODE_ENV=production
ENV DEBUG=false

RUN npm ci --quiet --only=production

CMD [ "node", "index.js" ]
