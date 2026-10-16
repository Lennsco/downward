#ifndef SEARCH_ALGORITHMS_PLAN_NEIGHBORHOOD_GRAPH_SEARCH_H
#define SEARCH_ALGORITHMS_PLAN_NEIGHBORHOOD_GRAPH_SEARCH_H

#include "../component.h"
#include "../search_algorithm.h"
#include <memory>
#include <string>
#include <vector>

namespace pngs {




class PNG_Search : public SearchAlgorithm{
    protected:
    virtual void initialize() override;
    virtual SearchStatus step() override;

    std::vector<std::shared_ptr<SearchAlgorithm>>
        task_independent_searches; //includes the option too first search for seed plan. 

    SearchStatus best_status; //Returns the stauts of the search
    void update_best_status(
        const std::shared_ptr<SearchAlgorithm> &task_specific_search);


    public:
    PNG_Search(
        const std::shared_ptr<AbstractTask> &task,
        const std::vector<std::shared_ptr<SearchAlgorithm>>
            &search,
        OperatorCost cost_type,
        int bound, double max_time, const std::string &description,
        utils::Verbosity verbosity);
        
    virtual void print_statistics() const override;
    virtual bool is_complete_within_bound() const override;

    

    };
 
}
#endif