#include "pngs.h"
#include "../plugins/plugin.h"
#include "../utils/component_errors.h"
#include "../utils/logging.h"

using namespace std;

namespace pngs{
PNG_Search::PNG_Search(
    const shared_ptr<AbstractTask> &task,
    const vector<shared_ptr<SearchAlgorithm>> &searches,
    OperatorCost cost_type, int bound,
    double max_time, const string &description, utils::Verbosity verbosity)
    : SearchAlgorithm( task, cost_type, bound, max_time, description, verbosity),
        task_independent_searches(searches),
        best_status(SearchStatus::FAILED){
    utils::verify_list_not_empty(searches, "searches");
    
    }

void PNG_Search::initialize(){
    log << "Plan Neighboring Graph Search, bound = " << bound << endl;

    State initial_state = state_registry.get_initial_state();
    SearchNode node = search_space.get_node(initial_state);
    node.open_initial();

}

SearchStatus PNG_Search::step(){
    State initial_state = state_registry.get_initial_state();
    SearchNode node = search_space.get_node(initial_state);
    node.close();
    statistics.inc_expanded();

    if(check_goal_and_set_plan(initial_state)){
        return SOLVED;
    }

    log << "PNGS search is implemented now as dummy, no expanding succ." << endl;
    return get_finished_search_status();

}

void PNG_Search::print_statistics() const{
    statistics.print_detailed_statistics();
    search_space.print_statistics();

}

bool PNG_Search::is_complete_within_bound() const{
    return best_status == SearchStatus::UNSOLVABLE_WITHIN_BOUND ||
           best_status == SearchStatus::UNSOLVABLE ||
           best_status == SearchStatus::SOLVED;

    // TODO: Bound has to be set with initial plan cost. After that only keep plans with cheaper cost, if yes replace bound, if not search till cheaper is found. 
}

void PNG_Search::update_best_status(
    const shared_ptr<SearchAlgorithm> &task_specific_search) {
    if (best_status == SearchStatus::SOLVED ||
        best_status == SearchStatus::UNSOLVABLE) {
        return;
    }
    SearchStatus current_search_status =
        task_specific_search->get_finished_search_status();

    switch (current_search_status) {
    case SearchStatus::SOLVED:
    case SearchStatus::UNSOLVABLE:
        best_status = current_search_status;
        break;
    case SearchStatus::UNSOLVABLE_WITHIN_BOUND:
        /*
           We ignore inner bounds if the inner search did not check up to the
           outer bound.
        */
        if (task_specific_search->get_bound() >= bound) {
            best_status = current_search_status;
        }
        break;
    case SearchStatus::FAILED:
    case SearchStatus::TIMEOUT:
    case SearchStatus::IN_PROGRESS:
        break;
    }
} // TODO:  NOT NEEDED YET. 

class PNGSearchFeature : public plugins::TypedFeature<TaskIndependentSearchAlgorithm> {
    public:
    PNGSearchFeature() : TypedFeature("pngs"){
        document_title("Plan Neighborhood Graph Search");
        document_synopsis("PNGS Skeleton");

        add_list_option<shared_ptr<TaskIndependentSearchAlgorithm>>(
            "searches", "search algorithms that returns an initial seed plan",
            "");

        add_search_algorithm_options_to_feature(*this, "pngs");

    }

    virtual shared_ptr<TaskIndependentSearchAlgorithm> create_component(
        const plugins::Options &opts) const override{
            return components::make_auto_task_independent_component<PNG_Search, SearchAlgorithm>(
                opts.get_list<shared_ptr<TaskIndependentSearchAlgorithm>>(
                "searches"),
                get_search_algorithm_arguments_from_options(opts));
    }
    
};

static plugins::FeaturePlugin<PNGSearchFeature> _plugin; //This should put PNGS into a global regestry 

    
}