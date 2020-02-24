#ifndef TRACK_H
#define TRACK_H

#include <map>

template<typename V>
class BaseTrack {
public:
    V& operator[](double t) { return track[t]; } 
    const V& operator[](double t) const { return track[t]; }

    auto begin() { return track.begin(); }
    auto end() { return track.end(); }
    
    auto begin() const { return track.cbegin(); }
    auto end() const { return track.cend(); }

    const V& approx(double t) const { return track.lower_bound(t)->second; }

private:
    std::map<double, V> track;
};

class DblTrack : public BaseTrack<double> {};
class BoolTrack : public BaseTrack<bool> {};

#endif // TRACK_H
