#include "VRSemanticUtils.h"
#include "VREntity.h"
#include "VROntology.h"
#include "VRReasoner.h"
#include "VRProperty.h"

#include <iostream>

using namespace OSG;

Variable::Variable() {;}

string Variable::toString() {
    string s = value+"(" + concept + "){";
    for (auto i : entities) s += i.second->getName()+",";
    if (entities.size() > 0) s.pop_back();
    s +="}[";
    if (isAnonymous) s += "anonymous, ";
    if (isAssumption) s += "assumption, ";
    s += valid ? "valid" : "invalid";
    s +="]";
    return s;
}

/* Variable flags:
    isAssumption: no entities of the type have been found
    isAnonymous: no entities with that name have been found
    !valid: something is terribly wrong!
*/

Variable::Variable(VROntologyPtr onto, string concept, string var) {
    auto cl = onto->getConcept(concept);
    if (cl == 0) return;

    if ( auto i = onto->getInstance(var) ) {
        addEntity(i);
        this->concept = concept; // TODO: maybe the entity has a concept that inherits from the concept passed above?
        isAnonymous = false;
    } else { // get all entities of the required type
        for (auto i : onto->getInstances(concept)) addEntity(i);
        if (entities.size() == 0) {
            auto i = onto->addInstance(var, concept);
            addEntity(i);
            isAssumption = true; // TODO: put that in the evaluation
        }
        this->concept = concept;
    }

    value = var;
    valid = true;
}

Variable::Variable(VROntologyPtr onto, string val) {
    value = val;
}

shared_ptr<Variable> Variable::create(VROntologyPtr onto, string concept, string var) { return shared_ptr<Variable>( new Variable(onto, concept, var) ); }
shared_ptr<Variable> Variable::create(VROntologyPtr onto, string val) { return shared_ptr<Variable>( new Variable(onto, val) ); }

void Variable::addEntity(VREntityPtr e) {
    entities[e->ID] = e;
    evaluations[e->ID] = Evaluation();
}

bool Variable::has(VariablePtr other, VROntologyPtr onto) {
    for (auto i1 : entities) { // all entities of that variable
        vector<VREntityPtr> matches;
        vector<VREntityPtr> toDiscard;
        for (auto i2 : other->entities) { // check each instance of the other variable

            bool res = false;
            for (auto p : i1.second->properties) { // all properties of each instance
                for (auto v : p.second) {
                    if (v->value == other->value) return true; // TODO: direct match with other variable value
                    if (v->value == i2.second->getName()) { res = true; matches.push_back(i2.second); }
                    if (res) break;
                }
                if (res) break;
            }
            if (!res) toDiscard.push_back(i2.second);
        }

        for (auto e : toDiscard) other->discard(e);
        if (matches.size() > 0) return true;
    }

    return false;
}

bool Variable::is(VariablePtr other, VPath& p1, VPath& p2) {
    if (!valid || !other->valid) return false;

    auto hasSameVal = [&](vector<string>& val1, vector<string>& val2){
        for (string s2 : val1) {
            for (string s3 : val2) {
                if (s2 == s3) return true;
            }
        }
        return false;
    };

    auto hasSameVal2 = [&](vector<string>& val1){
        for (auto i2 : other->entities) {
            vector<string> val2 = i2.second->getAtPath(p2.nodes);
            if (hasSameVal(val1, val2)) return true;
        }

        for (string s : val1) if (s == other->value) return true;
        return false;
    };

    bool res = false;
    for (auto i1 : entities) {
        vector<string> val1 = i1.second->getAtPath(p1.nodes);
        auto r = hasSameVal2(val1);
        if (!r) evaluations[i1.first].state = Evaluation::INVALID;
        if (r) res = true;
    }
    return res;
}

bool Variable::operator==(Variable v) {
    if (!v.valid || !valid) return false;
    for (uint i=0; i<entities.size(); i++) {
        if (v.entities[i] != entities[i]) return false;
    }
    return true;
}

void Variable::discard(VREntityPtr e) {
    if (!entities.count(e->ID)) return;
    entities.erase(e->ID);
    evaluations.erase(e->ID);
}

VPath::VPath(string p) {
    nodes = VRReasoner::split(p, '.');
    root = nodes[0];
    first = nodes[nodes.size()-1];
}

string VPath::toString() {
    string s;
    for (auto p : nodes) s += p + '.';
    if (nodes.size() > 0) s.pop_back();
    return s;
}

Context::Context() {}

Context::Context(VROntologyPtr onto) {
    this->onto = onto;

    cout << "Init context:" << endl;
    for (auto i : onto->instances) {
        if (i.second->getConcepts().size() == 0) { cout << "Context::Context instance " << i.second->getName() << " has no concepts!" << endl; continue; }
        vars[i.second->getName()] = Variable::create( onto, i.second->getConcepts()[0]->getName(), i.second->getName() );
        cout << " add instance " << i.second->toString() << endl;
    }

    for ( auto r : onto->getRules()) {
        cout << "Context prep rule " << r->toString() << endl;
        Query q(r->toString());
        if (!q.request) continue;

        for (Term& t : q.request->terms) {
            t.var = Variable::create(onto,t.path.root);

            for (auto& s : q.statements) {
                if (s->isSimpleVerb()) continue;
                s->terms[0].var = Variable::create(onto, s->terms[0].path.root);
                auto var = s->terms[0].var;
                if (var->value != t.var->value) continue;

                t.var->concept = s->verb;
                cout << " Set concept of " << t.var->value << " to " << s->verb << endl;
                break;
            }
        }

        rules[r->toString()] = q;
    }
}

// TODO: parse concept statements here
Query::Query() {}

Query::Query(string q) {
    vector<string> parts = VRReasoner::split(q, ':');
    if (parts.size() > 0) request = VRStatement::create(parts[0]);
    if (parts.size() > 1) {
        parts = VRReasoner::split(parts[1], ';');
        for (int i=0; i<parts.size(); i++) statements.push_back(VRStatement::create(parts[i], i));
    }
}

string Query::toString() {
    string res = request->toString() + " :";
    for (auto s : statements) res += " " + s->toString();
    return res;
}

Term::Term(string s) : path(s), str(s) {;}

bool Term::valid() { return var->valid; }

void Query::checkState() {
    int r = 1;
    for (auto i : statements) if(i->state == 0) r = 0;
    request->state = r;
}
