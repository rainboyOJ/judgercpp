/**
 * 对socket进行管理,
 * - 添加
 * - 删除
 * - 获取
 */

#include <atomic>
#include <vector>
#include <map>
#include <mutex>

class socketManager {
public:
    void insert(int fd);       //插入
    bool get(int &fd);         //获得
    bool remove(int fd);       //删除
    bool unuse(int fd);        //设末使用
    void removeAll();
private:
    std::map<int,bool> socket_map; // {fd,use?}
    std::mutex mtx;
};

void socketManager::insert(int fd){
    std::lock_guard<std::mutex> lock(mtx);
    socket_map.emplace(fd,false);
}

bool socketManager::get(int &fd){
    std::lock_guard<std::mutex> lock(mtx);
    for (auto& e : socket_map) {
        if( e.second == false){
            e.second = true;
            fd = e.first;
            return true;
        }
    }
    return false;
}

bool socketManager::remove(int fd){
    std::lock_guard<std::mutex> lock(mtx);
    auto it = socket_map.find(fd);
    if( it == socket_map.end())
        return false;       //dont find
    if( it->second == true)
        return true;        //using
    else{
        socket_map.erase(it);
        return true;
    }
    return false;
}

bool socketManager::unuse(int fd){
    std::lock_guard<std::mutex> lock(mtx);
    auto it = socket_map.find(fd);
    if( it == socket_map.end())
        return true;       //dont find
    if( it->second == false )
        return true;        //using
    return false;
}

void socketManager::removeAll(){
    std::lock_guard<std::mutex> lock(mtx);
    for( auto it = socket_map.begin() ;it != socket_map.end() ;){
        it = socket_map.erase(it);
    }
}
