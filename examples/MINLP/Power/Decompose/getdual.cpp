//
//  Decompose.cpp
//  Gravity
//
//  Created by Guanglei Wang on 6/9/17.
//
//

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <cstring>
#include <fstream>
#include "../PowerNet.h"
#include <gravity/model.h>
#include <gravity/solver.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef USE_BOOST
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <deque>
#include <iterator>
#endif


// CLIQUE BASED REFORMULATION
using namespace std;
using namespace gravity;
Net* get_cliquetree(Net* grid){
#ifdef USE_BOOST
    /** Note that we also need the edge information of the clique tree **/
    /** boost graph library or implement the expanded version of MCS algorithm by Blair and Peyton */
    typedef boost::adjacency_list <boost::vecS,
            boost::vecS,
            boost::undirectedS,
            boost::no_property,
            boost::property < boost::edge_weight_t, int >
            > Graph;
    typedef boost::graph_traits <Graph>::edge_descriptor Edge;
    typedef boost::graph_traits <Graph>::vertex_descriptor Vertex;

    // BUILD THE INTERSECTION GRAPH OF THE CLIQUES
    typedef std::pair<int, int> E;
    std::vector<E> edges;
    std::vector<int> weights;
    int nb_cliques = grid->_bags.size();
    for (int i = 0; i < nb_cliques; i++) {
        cout << "bag " << i << " has " << grid->_bags[i].size() << " vertices." <<endl;
        sort(grid->_bags[i].begin(), grid->_bags[i].end());
        for (int j = i +1; j < nb_cliques; j++) {
            vector<Node*> v3;
            sort(grid->_bags[j].begin(), grid->_bags[j].end());
            set_intersection(grid->_bags[i].begin(), grid->_bags[i].end(), grid->_bags[j].begin(), grid->_bags[j].end(), back_inserter(v3));
            if (v3.size() > 0) {
                edges.push_back(E(i, j));
                weights.push_back(-v3.size());
            }
        }
    }
    size_t num_edges = edges.size();

#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
    Graph g(num_nodes);
    boost::property_map<Graph, edge_weight_t>::type weightmap = get(edge_weight, g);
    for (std::size_t j = 0; j < num_edges; ++j) {
        Edge e;
        bool inserted;
        boost::tie(e, inserted) = boost::add_edge(edges[j].first, edges[j].second, g);
        boost::weightmap[e] = weights[j];
    }
#else
    Graph g(edges.begin(), edges.end(), weights.begin(), nb_cliques);
#endif
    boost::property_map < Graph, boost::edge_weight_t >::type weight = get(boost::edge_weight, g);
    std::vector < Edge > spanning_tree;
    boost::kruskal_minimum_spanning_tree(g, std::back_inserter(spanning_tree));

    DebugOn("Print the total " << spanning_tree.size() << " edges in the clique tree:" << endl);

    //////////CLIQUE TREE /////////////////////////////
    Net* cliquetree = new Net();
    Node* node = nullptr;
    Arc*  a = nullptr;
    string name;
    for (int i = 0; i < nb_cliques; i++) {
        node= new Node(to_string(i), i);
        cliquetree->add_node(node);
    }

    for (std::vector < Edge >::iterator ei = spanning_tree.begin();
            ei != spanning_tree.end(); ++ei) {
        int u = source(*ei, g);
        int v = target(*ei, g);
        DebugOn(u << " <--> " << v
                << " with weight of " << -weight[*ei]
                << endl);
        name = (int) cliquetree->arcs.size();
        a = new Arc(name);
        a->_id = cliquetree->arcs.size();

        // intersection
        vector<Node*> v3;
        sort(grid->_bags[u].begin(), grid->_bags[u].end());
        sort(grid->_bags[v].begin(), grid->_bags[v].end());
        set_intersection(grid->_bags[u].begin(), grid->_bags[u].end(),
                         grid->_bags[v].begin(), grid->_bags[v].end(),
                         back_inserter(v3));

        a->_src = cliquetree->get_node(to_string(u));
        a->_dest = cliquetree->get_node(to_string(v));
        a->_weight = -weight[*ei];
        a->_intersection = v3;
        cliquetree->add_arc(a);
        a->connect();
    }
    return cliquetree;
#endif
}

void getdual_relax(PowerNet* grid)
{

    // Grid Parameters
    auto bus_pairs = grid->get_bus_pairs();
    auto nb_bus_pairs = bus_pairs.size();
    auto nb_gen = grid->get_nb_active_gens();
    auto nb_lines = grid->get_nb_active_arcs();
    auto nb_buses = grid->get_nb_active_nodes();
    DebugOn("nb gens = " << nb_gen << endl);
    DebugOn("nb lines = " << nb_lines << endl);
    DebugOn("nb buses = " << nb_buses << endl);
    DebugOn("nb bus_pairs = " << nb_bus_pairs << endl);

    // Schedule
    unsigned T = 1;
    param<Real> rate_ramp("rate_ramp");
    param<Real> rate_switch("rate_switch");
    param<Real> min_up("min_up");
    param<Real> min_down("min_down");
    param<Real> cost_up("cost_up");
    param<Real> cost_down("cost_down");
    for (auto g: grid->gens) {
        rate_ramp(g->_name) = max(grid->pg_min(g->_name).getvalue(), 0.25*grid->pg_max(g->_name).getvalue());
        rate_ramp._dim++;
        rate_switch(g->_name) = max(grid->pg_min(g->_name).getvalue(), 0.25*grid->pg_max(g->_name).getvalue());
        rate_switch._dim++;
    }
    min_up = 1;
    min_down = 1;
    cost_up = 50;
    cost_down = 30;

    grid->c0.time_expand(T);
    grid->c1.time_expand(T);
    grid->c2.time_expand(T);
    grid->S_max.time_expand(T);
    grid->tan_th_min.time_expand(T);
    grid->tan_th_max.time_expand(T);
    grid->g_tt.time_expand(T);
    grid->g_ff.time_expand(T);
    grid->g_ft.time_expand(T);
    grid->g_tf.time_expand(T);
    grid->b_tt.time_expand(T);
    grid->b_ff.time_expand(T);
    grid->b_ft.time_expand(T);
    grid->b_tf.time_expand(T);
    grid->pg_min.time_expand(T);
    grid->pg_max.time_expand(T);
    grid->qg_min.time_expand(T);
    grid->qg_max.time_expand(T);
    grid->w_min.time_expand(T);
    grid->w_max.time_expand(T);
    rate_ramp.time_expand(T);
    rate_switch.time_expand(T);

    /** Clique tree decomposition **/

    Net* chordal = grid->get_chordal_extension();
    grid->get_clique_tree();
    const unsigned nb_cliques = grid->_bags.size();

    vector<vector<Bus*>> bag_bus; // Note that each clique contains just nodes, not buses!
    vector<vector<Gen*>> bag_gens;
    vector<vector<Arc*>> bag_arcs; //bag_arcs contains the arcs of the power grid while variables associated with W are defined on chordal graph.

    for (int c = 0; c < nb_cliques; c++) {
        vector<Bus*> bag_B;
        vector<Gen*> bag_G;
        vector<Arc*> bag_A;
        for (int i = 0; i < grid->_bags[c].size(); i++) {
            Bus* bus = (Bus*) grid->get_node(grid->_bags[c].at(i)->_name);
            if (bus !=nullptr) {
                bag_B.push_back(bus);
            }
            if (bus->_has_gen) {
                bag_G.insert(bag_G.end(), bus->_gen.begin(), bus->_gen.end());
            }
            for (int j = i+1; j < grid->_bags[c].size(); j++) {
                Arc* a = (Arc*)grid->get_arc(bus, grid->get_node(grid->_bags[c].at(j)->_name));
                if (a != nullptr)
                    bag_A.push_back(a);
            }
        }
        bag_bus.push_back(bag_B);
        bag_gens.push_back(bag_G);
        bag_arcs.push_back(bag_A);
    }
    bag_bus.resize(nb_cliques);
    bag_arcs.resize(nb_cliques);
    bag_gens.resize(nb_cliques);

    Net* cliquetree = get_cliquetree(grid);

    /** build model */
    Model ACUC("ACUC Model");

    /** Variables */
    // power generation
    var<Real> Pg("Pg", grid->pg_min.in(grid->gens, T), grid->pg_max.in(grid->gens, T));
    var<Real> Qg ("Qg", grid->qg_min.in(grid->gens, T), grid->qg_max.in(grid->gens, T));
    ACUC.add_var(Pg^(T*nb_gen));
    ACUC.add_var(Qg^(T*nb_gen));

    // power flow
    var<Real> Pf_from("Pf_from", grid->S_max.in(grid->arcs, T));
    var<Real> Qf_from("Qf_from", grid->S_max.in(grid->arcs, T));
    var<Real> Pf_to("Pf_to", grid->S_max.in(grid->arcs, T));
    var<Real> Qf_to("Qf_to", grid->S_max.in(grid->arcs, T));
    ACUC.add_var(Pf_from^(T*nb_lines));
    ACUC.add_var(Qf_from^(T*nb_lines));
    ACUC.add_var(Pf_to^(T*nb_lines));
    ACUC.add_var(Qf_to^(T*nb_lines));

    // Lifted variables.
    var<Real>  R_Wij("R_Wij", grid->wr_min.in(bus_pairs, T), grid->wr_max.in(bus_pairs, T)); // real part of Wij
    var<Real>  Im_Wij("Im_Wij", grid->wi_min.in(bus_pairs, T), grid->wi_max.in(bus_pairs, T)); // imaginary part of Wij.
    var<Real>  Wii("Wii", grid->w_min.in(grid->nodes, T), grid->w_max.in(grid->nodes, T));
    ACUC.add_var(Wii^(T*nb_buses));
    ACUC.add_var(R_Wij^(T*nb_bus_pairs));
    ACUC.add_var(Im_Wij^(T*nb_bus_pairs));
    R_Wij.initialize_all(1.0);
    Wii.initialize_all(1.001);

    // Commitment variables
    var<bool>  On_off("On_off", 0, 1);
    var<Real>  Start_up("Start_up", 0, 1);
    var<Real>  Shut_down("Shut_down", 0, 1);
    ACUC.add_var(On_off^(T*nb_gen));
    ACUC.add_var(Start_up^(T*nb_gen));
    ACUC.add_var(Shut_down^(T*nb_gen));

    /* Construct the objective function*/
    func_ obj;
    for (auto g:grid->gens) {
        if (g->_active) {
            for (int t = 0; t < T; t++) {
                if (t > 1) {
                    string l = to_string(t);
                    obj += grid->c1(g->_name, l)*Pg(g->_name, l) + grid->c2(g->_name, l)*Pg(g->_name, l)*Pg(g->_name, l) + grid->c0(g->_name, l);
                    //obj += cost_up.getvalue()*Start_up(g->_name, l)+ cost_down.getvalue()*Shut_down(g->_name, l);
                }
                else {
                    obj += grid->c1(g->_name)*Pg(g->_name) + grid->c2(g->_name)*Pg(g->_name)*Pg(g->_name) + grid->c0(g->_name);
                    //obj += cost_up.getvalue()*Start_up(g->_name)+ cost_down.getvalue()*Shut_down(g->_name);
                }
            }
        }
    }
    //obj  = sum(grid->c0.in(grid->gens, T));
    //obj += sum(grid->c1.in(grid->gens, T), Pg.in(grid->gens, T));
    //obj += sum(grid->c2.in(grid->gens, T), power(Pg.in(grid->gens, T), 2));
    //obj += cost_up.getvalue()*sum(Start_up.in(grid->gens, T))+ cost_down.getvalue()*sum(Shut_down.in(grid->gens,T));
    ACUC.set_objective(min(obj));

    /** Define constraints */
    /* SOCP constraints */
    Constraint SOC("SOC");
    SOC =  power(R_Wij.in(bus_pairs, T), 2) + power(Im_Wij.in(bus_pairs, T), 2) - Wii.from(bus_pairs, T)*Wii.to(bus_pairs, T) ;
    ACUC.add_constraint(SOC <= 0);
    //KCL
    for (int t = 0; t < T; t++)
        for (auto b: grid->nodes) {
            if (!b->_active) {
                continue;
            }
            Bus* bus = (Bus*) b;
            Constraint KCL_P("KCL_P"+bus->_name+ "time_" + to_string(t));
            Constraint KCL_Q("KCL_Q"+bus->_name+ "time_" + to_string(t));

            /* Power Conservation */
            KCL_P  = sum(Pf_from.in_at(b->get_out(), t)) + sum(Pf_to.in_at(b->get_in(), t)) + bus->pl()- sum(Pg.in_at(bus->_gen, t));
            KCL_Q  = sum(Qf_from.in_at(b->get_out(), t)) + sum(Qf_to.in_at(b->get_in(), t)) + bus->ql()- sum(Qg.in_at(bus->_gen, t));

            /* Shunts */
            KCL_P +=  bus->gs()*Wii(bus->_name);
            KCL_Q -=  bus->bs()*Wii(bus->_name);

            ACUC.add_constraint(KCL_P = 0);
            ACUC.add_constraint(KCL_Q = 0);
        }

    //AC Power Flow.
    Constraint Flow_P_From("Flow_P_From");
    Flow_P_From += Pf_from.in(grid->arcs, T);
    Flow_P_From -= grid->g_ff.in(grid->arcs, T)*Wii.from(grid->arcs, T);
    Flow_P_From -= grid->g_ft.in(grid->arcs, T)*R_Wij.in_pairs(grid->arcs, T);
    Flow_P_From -= grid->b_ft.in(grid->arcs, T)*Im_Wij.in_pairs(grid->arcs, T);
    ACUC.add_constraint(Flow_P_From = 0);

    Constraint Flow_P_To("Flow_P_To");
    Flow_P_To += Pf_to.in(grid->arcs, T);
    Flow_P_To -= grid->g_tt.in(grid->arcs, T)*Wii.to(grid->arcs, T);
    Flow_P_To -= grid->g_tf.in(grid->arcs, T)*R_Wij.in_pairs(grid->arcs, T);
    Flow_P_To += grid->b_tf.in(grid->arcs, T)*Im_Wij.in_pairs(grid->arcs, T);
    ACUC.add_constraint(Flow_P_To = 0);

    Constraint Flow_Q_From("Flow_Q_From");
    Flow_Q_From += Qf_from.in(grid->arcs, T);
    Flow_Q_From += grid->b_ff.in(grid->arcs, T)*Wii.from(grid->arcs, T);
    Flow_Q_From += grid->b_ft.in(grid->arcs, T)*R_Wij.in_pairs(grid->arcs, T);
    Flow_Q_From += grid->g_ft.in(grid->arcs, T)*Im_Wij.in_pairs(grid->arcs, T);
    ACUC.add_constraint(Flow_Q_From = 0);

    Constraint Flow_Q_To("Flow_Q_To");
    Flow_Q_To += Qf_to.in(grid->arcs, T);
    Flow_Q_To += grid->b_tt.in(grid->arcs, T)*Wii.to(grid->arcs, T);
    Flow_Q_To += grid->b_tf.in(grid->arcs, T)*R_Wij.in_pairs(grid->arcs, T);
    Flow_Q_To -= grid->g_tf.in(grid->arcs, T)*Im_Wij.in_pairs(grid->arcs, T);
    ACUC.add_constraint(Flow_Q_To = 0);

    /* Phase Angle Bounds constraints */
    Constraint PAD_UB("PAD_UB");
    PAD_UB = Im_Wij.in(bus_pairs, T);
    PAD_UB -= (grid->tan_th_max).in(bus_pairs, T)*R_Wij.in(bus_pairs, T);
    ACUC.add_constraint(PAD_UB <= 0);

    Constraint PAD_LB("PAD_LB");
    PAD_LB =  Im_Wij.in(bus_pairs, T);
    PAD_LB -= grid->tan_th_min.in(bus_pairs, T)*R_Wij.in(bus_pairs, T);
    ACUC.add_constraint(PAD_LB >= 0);

    /* Thermal Limit Constraints */
    Constraint Thermal_Limit_from("Thermal_Limit_from");
    Thermal_Limit_from += power(Pf_from.in(grid->arcs, T),  2) + power(Qf_from.in(grid->arcs, T), 2);
    Thermal_Limit_from -= power(grid->S_max.in(grid->arcs, T), 2);
    ACUC.add_constraint(Thermal_Limit_from <= 0);

    Constraint Thermal_Limit_to("Thermal_Limit_to");
    Thermal_Limit_to += power(Pf_to.in(grid->arcs, T), 2) + power(Qf_to.in(grid->arcs, T), 2);
    Thermal_Limit_to -= power(grid->S_max.in(grid->arcs, T),2);
    ACUC.add_constraint(Thermal_Limit_to <= 0);

    /* Commitment constraints */
    // Inter-temporal constraints
    for (int t = 1; t < T; t++) {
        Constraint MC1("MC1_"+ to_string(t));
        Constraint MC2("MC2_"+ to_string(t));
        MC1 = On_off.in_at(grid->gens, t)- On_off.in_at(grid->gens, t-1)-  Start_up.in_at(grid->gens, t);
        MC2 = On_off.in_at(grid->gens, t-1) - On_off.in_at(grid->gens, t) - Shut_down.in_at(grid->gens, t);
        ACUC.add_constraint(MC1 <= 0);
        ACUC.add_constraint(MC2 <= 0);
    }

    // Min-up constraints
    for (int t = 1; t < T; t++) {
        Constraint Min_up1("Min_up1_"+ to_string(t));
        Min_up1 = On_off.in_at(grid->gens, t) - On_off.in_at(grid->gens, t-1) - Start_up.in_at(grid->gens, t) + Shut_down.in_at(grid->gens, t);
        ACUC.add_constraint(Min_up1 = 0);
    }

    for (int t = min_up.getvalue(); t < T; t++) {
        Constraint Min_Up("Min_Up_constraint" + to_string(t));
        for (int l = t-min_up.getvalue()+1; l < t +1; l++) {
            Min_Up   += Start_up.in_at(grid->gens, l);
        }
        Min_Up -= On_off.in_at(grid->gens, t);
        ACUC.add_constraint(Min_Up <= 0);
    }

    for (int t = min_down.getvalue(); t < T; t++) {
        Constraint Min_Down("Min_Down_constraint" + to_string(t));
        for (int l = t-min_down.getvalue()+1; l < t +1; l++) {
            Min_Down   += Shut_down.in_at(grid->gens, l);
        }
        Min_Down -= 1 - On_off.in_at(grid->gens, t);
        ACUC.add_constraint(Min_Down <= 0);
    }

    //Ramp rate
    Constraint Production_P_LB("Production_P_LB");
    Constraint Production_P_UB("Production_P_UB");
    Constraint Production_Q_LB("Production_Q_LB");
    Constraint Production_Q_UB("Production_Q_UB");

    Production_P_UB = Pg.in(grid->gens, T) - grid->pg_max.in(grid->gens, T)*On_off.in(grid->gens,T);
    Production_P_LB = Pg.in(grid->gens, T) - grid->pg_min.in(grid->gens, T)*On_off.in(grid->gens,T);
    ACUC.add_constraint(Production_P_UB <=0);
    ACUC.add_constraint(Production_P_LB >= 0);

    grid->qg_max.print(true);
    grid->qg_min.print(true);

    Production_Q_UB = Qg.in(grid->gens, T) - grid->qg_max.in(grid->gens, T)*On_off.in(grid->gens,T);
    Production_Q_LB = Qg.in(grid->gens, T) - grid->qg_min.in(grid->gens, T)*On_off.in(grid->gens,T);
    ACUC.add_constraint(Production_Q_UB <= 0);
    ACUC.add_constraint(Production_Q_LB >= 0);

    for (int t = 1; t < T; t++) {
        Constraint Ramp_up("Ramp_up_constraint" + to_string(t));
        Constraint Ramp_down("Ramp_down_constraint" + to_string(t));

        Ramp_up = Pg.in_at(grid->gens, t);
        Ramp_up -= Pg.in_at(grid->gens, t-1);
        Ramp_up -= rate_ramp*On_off.in_at(grid->gens, t-1);
        Ramp_up -= rate_switch*(1 - On_off.in_at(grid->gens, t));

        Ramp_down = Pg.in_at(grid->gens, t-1);
        Ramp_down -= Pg.in_at(grid->gens, t);
        Ramp_down -= rate_ramp*On_off.in_at(grid->gens, t);
        Ramp_down -= rate_switch*(1 - On_off.in_at(grid->gens, t-1));

        ACUC.add_constraint(Ramp_up <= 0);
        ACUC.add_constraint(Ramp_down <= 0);

    }

    /* Resolve it! */
    solver relax(ACUC,ipopt);
    relax.run();
}
