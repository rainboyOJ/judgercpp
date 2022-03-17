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
    bool get(int &fd);         //获得随机的socket
    bool get_specified_fd(int fd);  //获取指定的socket
    bool remove(int fd);       //删除
    bool unuse(int fd);        //设末使用
    void removeAll();
private:
    std::map<int,bool> socket_map; // {fd,use?}
    std::mutex mtx;
};

class socketManagerRAII{
public:
    socketManagerRAII(socketManager &sm) : _SM{sm}
    { //随机获取
        while( _SM.get(fd)  == false) ;
    }
    //指定获取
    socketManagerRAII(socketManager &sm,int _fd) : _SM{sm}
    { 
        while( _SM.get_specified_fd(_fd)  == false) ;
        fd = _fd;
    }
    ~socketManagerRAII(){ _SM.unuse(fd); }
    int get() { return fd; }
private:
    socketManager & _SM;
    int fd{-1};
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
    if( it == socket_map.end() )
        return false;       //dont find
    if( it->second == true)
        return false;        //using
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
    it->second = false; //一定把这个设为 false
    return true;       //using
}

void socketManager::removeAll(){
    std::lock_guard<std::mutex> lock(mtx);
    for( auto it = socket_map.begin() ;it != socket_map.end() ;){
        it = socket_map.erase(it);
    }
}

bool socketManager::get_specified_fd(int fd){
    std::lock_guard<std::mutex> lock(mtx);
    auto it = socket_map.find(fd);
    if( it == socket_map.end() || it->second == true)
        return false;       //dont find
    else{
        it->second = true; //设为正在使用
        return true;    //获取成功
    }
}

