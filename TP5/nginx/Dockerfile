# Use of a lightweight nginx image
FROM nginx:alpine-slim

# Copy the config of nginx inside the container
COPY nginx.conf /etc/nginx/nginx.conf

# Documentation
LABEL AUTHOR="François Gibier"

# No need to provide CMD because nginx image already has one (we can override if we need though)