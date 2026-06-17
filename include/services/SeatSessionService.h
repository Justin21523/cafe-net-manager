#pragma once

class SeatRepository;
class SeatSessionRepository;

class SeatSessionService {
public:
    SeatSessionService(SeatRepository *seatRepo, SeatSessionRepository *sessionRepo);

    bool startSession(int seatId);
    bool endSession(int seatId);

private:
    SeatRepository *m_seatRepo;
    SeatSessionRepository *m_sessionRepo;
};