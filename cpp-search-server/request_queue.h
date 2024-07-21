#pragma once

#include "search_server.h"

#include <deque>

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);
    // сделаем "обёртки" для всех методов поиска, чтобы сохранять результаты для нашей статистики
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);

    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);

    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;
private:
    struct QueryResult {
        int time_result_;
        int count_results_;
        QueryResult(int time_result, int count_results);
    };
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    const SearchServer& search_server_;
    int time_ = 0;
}; 

template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
    std::vector<Document> documents = search_server_.FindTopDocuments(raw_query, document_predicate);
    if(time_ == min_in_day_){
        requests_.pop_front();
        requests_.push_back(QueryResult(time_, documents.size()));
    }
    else if(time_ < min_in_day_){
        requests_.push_back(QueryResult(time_, documents.size()));
        time_++;
    }
    return documents;
}