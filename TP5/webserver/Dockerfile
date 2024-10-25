# Build environment using a lightweight node image (alpine)
FROM node:alpine AS build

# Copy application files into container
COPY app /app
WORKDIR /app

# Remove all files without js or json extension
RUN find . -type f ! -iname "*.js" -and ! -iname "*.json" -delete

# Install production dependencies only
RUN npm install --omit=dev


# Run environment using a lightweight image (alpine)
FROM alpine:latest

# Install nodejs
RUN apk add nodejs

# Copy applications and it's dependencies
COPY --from=build app /app
WORKDIR /app

# Remove useless files
RUN rm package.json package-lock.json

# Documentation
EXPOSE 8080
LABEL AUTHOR="François Gibier"

# Run the application (CMD more flexible than ENTRYPOINT)
CMD ["node", "src/index.js"]