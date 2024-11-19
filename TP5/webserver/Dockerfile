# Build image
FROM alpine AS build

WORKDIR /app

# Install required libraries to build app
RUN apk --no-cache add libc-dev gcc make libpq-dev

# Include application files
COPY include ./include
COPY src ./src
COPY makefile ./

# Build application
RUN make http


# Runtime image
FROM alpine

# Install required libraries to run the application
RUN apk --no-cache add libpq-dev

WORKDIR /app

# Copy exectutable from previous build
COPY --from=build app/http .

# Copy certs and files served by server
COPY cert ./cert
COPY public ./public

# Set the default env variables
ENV POSTGRES_PORT=5432
ENV POSTGRES_URL=localhost
ENV SERVER_PORT=8080

# Documentation of the exposed port
EXPOSE $POSTGRES_PORT
LABEL AUTHOR="François Gibier"

# Start the server
CMD ["./http", "8080"]
