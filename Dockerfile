FROM gcc:13
WORKDIR /app
COPY . .
RUN g++ -std=c++17 -pthread -Iinclude src/main.cpp src/Server.cpp src/RequestHandler.cpp -o smartgym_server
EXPOSE 8080
CMD ["./smartgym_server"]