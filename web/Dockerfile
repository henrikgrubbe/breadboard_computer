# Stage 1: Compile and build app

# Use official node image as the base image
FROM node:22-alpine as appBuild

# Set the working directory
WORKDIR /usr/local/app

# Add the source code to app
COPY ./app /usr/local/app

# Install all dependencies
RUN npm ci --quiet

# Generate the build of the application
RUN npm run build


# Stage 2: Compile and build server

# Use official node image as the base image
FROM node:22-alpine as serverBuild

# Set the working directory
WORKDIR /usr/local/server

# Add the source code to app
COPY ./server /usr/local/server

# Install all dependencies
RUN npm ci --quiet

# Generate the build of the application
RUN npm run build

# Copy the build output
COPY --from=serverBuild /usr/local/app/dist /srv/http


# Stage 3: Run server
# Use official node image as the base image
FROM node:22-alpine

WORKDIR /server

ENV NODE_ENV=production
ENV DEBUG=false

COPY --from=serverBuild /usr/local/server/dist .
COPY --from=appBuild /usr/local/app/dist ./app

COPY .server/package*.json ./

RUN npm ci --quiet --only=production

CMD [ "node", "index.js" ]
