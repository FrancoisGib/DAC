FROM node:alpine as build

COPY app /app
WORKDIR /app

ENV NODE_ENV=production

EXPOSE 8080
RUN npm install --omit=dev

FROM gcr.io/distroless/nodejs18-debian12

COPY --from=build /app/src/index.js /index.js

CMD ["index.js"]