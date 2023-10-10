FROM node
WORKDIR /usr/src/app
RUN mkdir -p /usr/src/app/data
COPY package*.json ./
RUN npm install
EXPOSE 3000
COPY . .
CMD [ "node", "main.js" ]

