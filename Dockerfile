# Use the official Node.js v14 image as the base image
FROM node:14

# Set the working directory to /app
WORKDIR /app

# Copy the package.json and package-lock.json files to the container
COPY package*.json ./

# Install the dependencies
RUN npm install && npm install -g http-server

# Copy the index.html file to the container
COPY index.html .

# Expose port 8007
EXPOSE 8007

# Start the server
CMD ["http-server", "-p", "8007"]
