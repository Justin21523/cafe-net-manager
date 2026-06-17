#pragma once

class SeatRepository;
class SeatSessionRepository;

class SeatSessionService {
public:
    SeatSessionService(SeatRepository *seatRepo, SeatSessionRepository *sessionRepo);
    
    bool startSession(int seatId, int prepaidMinutes = 0, int hourlyRate = 0);
    bool endSession(int seatId);
    bool extendSession(int sessionId, int additionalMinutes); // Add this
    bool updateSessionTime(int sessionId); // Add this

private:
    SeatRepository *m_seatRepo;
    SeatSessionRepository *m_sessionRepo;
       
};