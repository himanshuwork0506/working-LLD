1. Code Flow
Initialize Trending Service
Record Video Views
Update Leaderboard Set
Evict Excess Videos
Render Console Output

2. Classes
ITrendingService: Service Interface - addView, getTopTrending
VideoTrendingService: Ranking Logic - addView, getTopTrending
ConsoleView: Output Display - renderTrending

3. Requirements
Functional:
Count Video Views
Maintain Top K List
Display Ranked Leaderboard

Non-Functional:
SOLID Principle Compliant
Efficient Sorting Mechanism

C++
#include <iostream>
#include <string>
#include <unordered_map>
#include <set>
#include <vector>

using namespace std;

// Standard contract for tracking and retrieving trending media.
class ITrendingService {
public:
    virtual void addView(const string& videoId) = 0;
    virtual vector<string> getTopTrending() = 0;
    
    // Ensures proper cleanup for derived classes.
    virtual ~ITrendingService() = default; 
};

// Implements ranking logic using hash maps and sets to maintain a strict Top K list.
class VideoTrendingService : public ITrendingService {
private:
    int limit;
    // Tracks the total views for every video.
    unordered_map<string, int> viewCounts;
    
    // Automatically sorts the highest scores using a balanced binary search tree.
    set<pair<int, string>> topK;

public:
    // Initializes the service with a maximum leaderboard capacity.
    VideoTrendingService(int k) : limit(k) {}

    // Increments view count and adjusts the leaderboard rankings.
    void addView(const string& videoId) override {
        int oldScore = viewCounts[videoId];
        int newScore = oldScore + 1;
        viewCounts[videoId] = newScore;

        // Removes the outdated score entry if it exists in the leaderboard.
        if (topK.find({oldScore, videoId}) != topK.end()) {
            topK.erase({oldScore, videoId});
        }
        
        // Inserts the newly incremented score.
        topK.insert({newScore, videoId});

        // Enforces the strict Top K limit by evicting the lowest score.
        if (topK.size() > limit) {
            topK.erase(topK.begin());
        }
    }

    // Retrieves the ranked list of videos formatted as strings.
    vector<string> getTopTrending() override {
        vector<string> result;
        // Iterates backwards to retrieve elements from highest to lowest.
        for (auto it = topK.rbegin(); it != topK.rend(); ++it) {
            result.push_back(it->second + " (" + to_string(it->first) + " views)");
        }
        return result;
    }
};

// Handles presentation logic, keeping I/O operations separate from core calculations.
class ConsoleView {
public:
    // Accepts any implementation of ITrendingService to render the leaderboard.
    static void renderTrending(ITrendingService* service) {
        vector<string> trendingList = service->getTopTrending();
        
        cout << "\n=== TRENDING NOW ===" << endl;
        for (int i = 0; i < trendingList.size(); ++i) {
            cout << i + 1 << ". " << trendingList[i] << endl;
        }
        cout << "====================\n" << endl;
    }
};

// Application entry point demonstrating data ingestion and rendering.
int main() {
    // 1. Initialize Trending Service
    ITrendingService* youtubeTrending = new VideoTrendingService(3);

    // 2. Record Video Views
    youtubeTrending->addView("Funny Cats");
    youtubeTrending->addView("Funny Cats");
    youtubeTrending->addView("Adobe Interview Prep");
    youtubeTrending->addView("Adobe Interview Prep");
    youtubeTrending->addView("Adobe Interview Prep");
    youtubeTrending->addView("Music Video");

    // 5. Render Console Output
    ConsoleView::renderTrending(youtubeTrending);

    delete youtubeTrending; 
    return 0;
}