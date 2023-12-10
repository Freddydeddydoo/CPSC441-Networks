#include <fstream>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include "iostream"
using namespace std;
#define BUFSIZE1 100

double bandWitdth = 6 * 125000;  //(5Mb/s)

enum EventType{
    ARRIVAL,
    DEPARTURE
};

struct Packet{
    int num;
    double arrivalTime;
    double size;
    double processTime;
    double totalTime; //process time + queue wait // in this scenario its basically the total time of the system
    double qTime; // queue wait
};

struct PacketHash {
    size_t operator()(const Packet& p) const {
        // Combine the hashes of arrivalTime, size, and processTime
        return std::hash<double>()(p.arrivalTime) ^ (std::hash<double>()(p.size) << 1) ^ (std::hash<double>()(p.processTime) << 2);
    }
};

// Equality operator for Packet
struct PacketEqual {
    bool operator()(const Packet& p1, const Packet& p2) const {
        // Compare arrivalTime, size, and processTime
        return p1.arrivalTime == p2.arrivalTime && p1.size == p2.size && p1.processTime == p2.processTime;
    }
};

struct Event {
    double timeStamp;
    EventType type;
    Packet packet;
};


struct CompareEvents {
    bool operator()(const Event& e1, const Event& e2) const {
        return e1.timeStamp > e2.timeStamp;
    }
};

queue<Packet> packetQueue;

int packetDropped = 0;
int totalPacket = 0;
int totalDelivered = 0;

int main() {
    vector<Packet> allPackets;
    vector<Event> allEvents;
    ifstream zoomPacketFile("stream.txt");
    double totalQueueDelay = 0.0;

    if (!zoomPacketFile) {
        cerr << "Uh oh, test.txt could not be opened for reading!\n";
        return 1;
    }

    Packet p;
    while (zoomPacketFile >> p.arrivalTime >> p.size) {
        p.num = totalPacket++;
        p.processTime = p.size / bandWitdth;
        p.totalTime = 0;
        p.qTime = 0;
        allPackets.push_back(p);
    }

    for (int i = 0; i < allPackets.size(); i++) {
        allPackets[i].totalTime = (i == 0) ? allPackets[i].processTime : allPackets[i - 1].totalTime + allPackets[i].processTime ;
        allPackets[i].qTime = (i == 0 || allPackets[i-1].totalTime <= allPackets[i].arrivalTime) ? 0 : allPackets[i-1].totalTime - allPackets[i].arrivalTime;


        Event eA{allPackets[i].arrivalTime, ARRIVAL, allPackets[i]};
        Event eD{allPackets[i].totalTime, DEPARTURE, allPackets[i]};
        allEvents.push_back(eA);
        allEvents.push_back(eD);
    }

    zoomPacketFile.close();

    sort(allEvents.begin(), allEvents.end(), [](const Event& a, const Event& b) {
        return a.timeStamp < b.timeStamp;
    });

    priority_queue<Event, vector<Event>, CompareEvents> eventQueue;
    for (const auto& event : allEvents) {
        eventQueue.push(event);
    }

    while (!eventQueue.empty()) {
        Event currentEvent = eventQueue.top();
        eventQueue.pop();

        if (currentEvent.type == ARRIVAL) {
            if (packetQueue.size() >= BUFSIZE1) {
                packetDropped++;
            } else {
                packetQueue.push(currentEvent.packet);
            }
        } else if (currentEvent.type == DEPARTURE) {
            if (!packetQueue.empty()) {
                Packet frontPacket = packetQueue.front();
                totalQueueDelay += currentEvent.packet.qTime;
                packetQueue.pop();
                totalDelivered++;
            }
        }
    }

    double averageQueueDelay = (totalQueueDelay / totalDelivered);
    double avgDropped =(double )  packetDropped / totalPacket;
    cout << "Total delivered : " << totalDelivered << endl;
    cout << "Total Packets Processed: " << totalPacket << endl;
    cout << "Total Packets Dropped: " << packetDropped << endl;
    cout << "Average Queue Delay: " << averageQueueDelay << " seconds" << endl;
    cout << "drop average for buffer size of " << BUFSIZE1 << " and wLan of " << bandWitdth << " is " << avgDropped;

    return 0;
}