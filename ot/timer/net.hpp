#ifndef OT_TIMER_NET_HPP_
#define OT_TIMER_NET_HPP_

#include <ot/spef/spef.hpp>
#include <ot/timer/pin.hpp>
#include <ot/traits.hpp>

namespace ot {

// Forward declaration
class RctEdge;
class RctNode;
class Rct;

// ------------------------------------------------------------------------------------------------

// Class: RctNode
class RctNode {

  friend class Rct;
  friend class Net;
  friend class Timer;
  
  public:

    RctNode() = default;
    RctNode(const std::string&);

    float load(Split, Tran) const;
    float cap(Split, Tran) const;
    float slew(Split, Tran, float) const;
    float delay(Split, Tran) const;

  private:

    std::string _name;                           

    std::array<std::array<float, MAX_TRAN>, MAX_SPLIT> _ures    {.0f, .0f, .0f, .0f}; 
    std::array<std::array<float, MAX_TRAN>, MAX_SPLIT> _ncap    {.0f, .0f, .0f, .0f};
    std::array<std::array<float, MAX_TRAN>, MAX_SPLIT> _load    {.0f, .0f, .0f, .0f}; 
    std::array<std::array<float, MAX_TRAN>, MAX_SPLIT> _beta    {.0f, .0f, .0f, .0f};
    std::array<std::array<float, MAX_TRAN>, MAX_SPLIT> _delay   {.0f, .0f, .0f, .0f};
    std::array<std::array<float, MAX_TRAN>, MAX_SPLIT> _ldelay  {.0f, .0f, .0f, .0f};
    std::array<std::array<float, MAX_TRAN>, MAX_SPLIT> _impulse {.0f, .0f, .0f, .0f};

    std::list<RctEdge*> _fanin;
    std::list<RctEdge*> _fanout;

    Pin* _pin {nullptr};

    void _scale_capacitance(float);
};

// ------------------------------------------------------------------------------------------------

// Class: RctEdge
class RctEdge {

  friend class Rct;
  friend class Net;
  friend class Timer;
  
  public:
    
    RctEdge(RctNode&, RctNode&, float);

    inline float res() const;
    inline void res(float);
  
  private:

    RctNode& _from;
    RctNode& _to;
    
    float _res {0.0f};

    void _scale_resistance(float);
};

// Function: res
inline float RctEdge::res() const {
  return _res;
}

// Procedure: res
inline void RctEdge::res(float v) {
  _res = v; 
}

// ------------------------------------------------------------------------------------------------

// Class: Rct
class Rct {

  friend class Net;
  friend class Timer;

  public:

    void update_rc_timing();
    void insert_segment(const std::string&, const std::string&, float);
    void insert_node(const std::string&, float = 0.0f);
    void insert_edge(const std::string&, const std::string&, float);
    
    float slew(const std::string&, Split, Tran, float) const;
    float delay(const std::string&, Split, Tran) const;

    inline size_t num_nodes() const;
    inline size_t num_edges() const;
    
    const RctNode* node(const std::string&) const;

  private:

    RctNode* _root {nullptr};

    std::unordered_map<std::string, RctNode> _nodes;
    std::list<RctEdge> _edges;

    void _update_load(RctNode*, RctNode*);
    void _update_delay(RctNode*, RctNode*);
    void _update_ldelay(RctNode*, RctNode*);
    void _update_response(RctNode*, RctNode*);
    void _scale_capacitance(float);
    void _scale_resistance(float);

    RctNode* _node(const std::string&);
};

// Function: num_nodes
inline size_t Rct::num_nodes() const {
  return _nodes.size();
}

// Function: num_edges
inline size_t Rct::num_edges() const {
  return _edges.size();
}

// ------------------------------------------------------------------------------------------------

// Class: Net
class Net {

  friend class Timer;
  friend class Arc;
  friend class Gate;
  friend class Pin;
  
  struct EmptyRct {
    std::array<std::array<float, MAX_TRAN>, MAX_SPLIT> load;
  };

  public:
    
    Net() = default;
    Net(const std::string&);

    inline const std::string& name() const;
    inline size_t num_pins() const;

  private:

    std::string _name;

    Pin* _root {nullptr};

    std::list<Pin*> _pins;

    std::variant<EmptyRct, Rct> _rct;

    bool _rc_timing_updated {false};

    float _load(Split, Tran) const;

    std::optional<float> _slew(Split, Tran, float, Pin&) const;
    std::optional<float> _delay(Split, Tran, Pin&) const;
    
    void _update_rc_timing();
    void _make_rct(const spef::Net&);
    void _insert_pin(Pin&);
    void _remove_pin(Pin&);
    void _scale_capacitance(float);
    void _scale_resistance(float);
}; 

// Function: name
inline const std::string& Net::name() const {
  return _name;
}

// Function: num_pins
inline size_t Net::num_pins() const {
  return _pins.size();
}

};  // end of namespace ot. -----------------------------------------------------------------------

#endif





