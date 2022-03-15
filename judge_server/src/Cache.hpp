/**
 * @desc 缓存
 * 实现添加与删除
 * 过期检查
 */

#include <vector>
#include <memory>
#include <map>
#include <atomic>
#include <ctime>

/**
 * 一个评测点的结果
 */
struct oneJudgeResult {
    
};


/**
 * 结果的集合
 */
struct judgeResult {
    explicit judgeResult(std::time_t expire)
    {
        time_stamp_ = std::time(nullptr) + expire;
    }
    void push_back(oneJudgeResult&& res){
        std::lock_guard<std::mutex> lock(mtx_);
        results.emplace_back(std::move(res));
    }
    std::vector<oneJudgeResult> results;
    std::time_t time_stamp_; //过期时间
    std::mutex mtx_;        //锁
};


struct Cache {
    Cache& get();       //单例模式
    void check_expire();
    std::shared_ptr<judgeResult> create_Results(std::string_view id); //创建一个结果集合
    std::weak_ptr<judgeResult> get_Results(); //得到一个结果集
    void remove(std::string_view id); //删除一个
private:
    Cache() = default;
    Cache(const Cache &) = delete;
    Cache(Cache &&)      = delete;

    std::map<std::string,std::shared_ptr<judgeResult>> map_;
    std::mutex mtx_;            //锁
    int max_age_ = 60*60; //最多60分钟
    std::atomic_int64_t id_ = 0;
};

std::shared_ptr<judgeResult> 
Cache::create_Results(std::string_view id) //创建一个结果集合
{
    auto s = std::make_shared<judgeResult>(max_age_);
    {
        std::lock_guard<std::mutex> lock(mtx_);
        map_.emplace(std::string(id),s);
    }
    return s;
}

std::weak_ptr<judgeResult>
Cache::get_Results(std::string id) //得到一个结果集
{
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = map_.find(std::string(id));
    return it != map_.end() ? it->second : nullptr;
}

void Cache::remove(std::string_view id) //删除一个
{
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = map_.find(std::string(id));
    if( it != map_.end())
        map_.erase(it);
}

void Cache::check_expire(){
    if(map_.empty()) return;
    auto now = std::time(nullptr);
    std::lock_guard<std::mutex> lock(mtx_);
    for (auto it = map_.begin(); it != map_.end();) {
        if (now - it->second->time_stamp()>= 0) {
            it = map_.erase(it);
        }
        else {
            ++it;
        }
    }
}
