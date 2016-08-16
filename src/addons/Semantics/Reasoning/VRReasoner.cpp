#include "VRReasoner.h"
#include "VROntology.h"
#include "VRProperty.h"

#include <iostream>
#include <sstream>
#include <list>
#include "core/utils/toString.h"

using namespace std;
using namespace OSG;

VRReasoner::VRReasoner() {;}

vector<string> VRReasoner::split(string s, string d) {
    vector<string> res;
    size_t pos = 0;
    int dN = d.length();

    string t;
    while ((pos = s.find(d)) != string::npos) {
        t = s.substr(0, pos);
        s.erase(0, pos + dN);
        res.push_back(t);
    }

    return res;
}

vector<string> VRReasoner::split(string s, char d) {
    vector<string> res;
    string t;
    stringstream ss(s);
    while (getline(ss, t, d)) res.push_back(t);
    return res;
}

bool VRReasoner::startswith(string s, string subs) {
    return s.compare(0, subs.size(), subs);
}

Variable::Variable() {;}

string Variable::toString() {
    string s = value+"("+concept+"){";
    for (auto i : instances) s += i.second->getName()+",";
    if (instances.size() > 0) s.pop_back();
    s +="}[";
    if (isAnonymous) s += "anonymous, ";
    if (isAssumption) s += "assumption, ";
    s += valid ? "valid" : "invalid";
    s +="]";
    return s;
}

/* Variable flags:
    isAssumption: no instances of the type have been found
    isAnonymous: no instances with that name have been found
    !valid: something is terribly wrong!
*/

Variable::Variable(VROntologyPtr onto, string concept, string var) {
    auto cl = onto->getConcept(concept);
    if (cl == 0) return;

    if ( auto i = onto->getInstance(var) ) {
        instances[i->ID] = i; // found an instance with the right name
        this->concept = i->concept->getName();
        isAnonymous = false;
    } else { // get all instances of the required type
            for (auto i : onto->getInstances(concept)) instances[i->ID] = i;
        if (instances.size() == 0) {
            auto i = onto->addInstance(var, concept);
            instances[i->ID] = i;
            isAssumption = true;
        }
        this->concept = concept;
    }

    value = var;
    valid = true;
}

Variable::Variable(VROntologyPtr onto, string val) {
    value = val;
    concept = "var";
    valid = true;
}

shared_ptr<Variable> Variable::create(VROntologyPtr onto, string concept, string var) { return shared_ptr<Variable>( new Variable(onto, concept, var) ); }
shared_ptr<Variable> Variable::create(VROntologyPtr onto, string val) { return shared_ptr<Variable>( new Variable(onto, val) ); }

bool Variable::operator==(Variable v) {
    if (!v.valid || !valid) return false;
    for (uint i=0; i<instances.size(); i++) {
        if (v.instances[i] != instances[i]) return false;
    }
    return true;
}

void Variable::discard(VREntityPtr e) {
    if (!instances.count(e->ID)) return;
    instances.erase(e->ID);
}

VPath::VPath(string p) {
    nodes = VRReasoner::split(p, '/');
    root = nodes[0];
    first = nodes[nodes.size()-1];
}

string VPath::toString() {
    string s;
    for (auto p : nodes) s += p + "/";
    if (nodes.size() > 0) s.pop_back();
    return s;
}

Context::Context() {}

Context::Context(VROntologyPtr onto) {
    this->onto = onto;

    cout << "Init context:" << endl;
    for (auto i : onto->instances) {
        if (!i.second->concept) { cout << "Context::Context instance " << i.second->getName() << " has no concept!" << endl; continue; }
        vars[i.second->getName()] = Variable::create( onto, i.second->concept->getName(), i.second->getName() );
        cout << " add instance " << i.second->getName() << " of concept " << i.second->concept->getName() << endl;
    }

    for ( auto r : onto->getRules()) {
        Query q(r->rule);

        cout << "Context prep rule " << r->rule << endl;
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

        rules[r->rule] = q;
    }
}

Statement::Statement() {;}

Statement::Statement(string s, int place) {
    this->place = place;
    auto s1 = VRReasoner::split(s, '(');
    auto v = VRReasoner::split(s1[0], '_');
    verb = v[0];
    verb_suffix = v.size() > 1 ? v[1] : "";
    auto s2 = VRReasoner::split( VRReasoner::split(s1[1], ')')[0] , ',');
    for (string s : s2) {
        terms.push_back(Term(s));
    }
}

StatementPtr Statement::New(string s, int p) { return StatementPtr( new Statement(s,p) ); }

string Statement::toString() {
    string s = verb + "(";
    for (auto t : terms) s += t.path.toString() + ",";
    if (terms.size() > 0) s.pop_back();
    s += ")";
    return s;
}

void Statement::updateLocalVariables(map<string, VariablePtr>& globals, VROntologyPtr onto) {
    for (auto& t : terms) {
        if (globals.count(t.path.root)) t.var = globals[t.path.root];
        else t.var = Variable::create(onto,t.path.root);
        //cout << "updateLocalVariables " << t.str << " " << t.var.value << " " << t.var.concept << endl;
    }
}

bool Statement::isSimpleVerb() {
    //static string verbs[] = {"q", "is", "is_any", "is_all", "is_not", "is_not_all", "has", "has_not"};
    static string verbs[] = {"q", "is", "has"};
    for (string v : verbs) if (v == verb) return true;
    return false;
}

bool Statement::match(StatementPtr s) {
    for (uint i=0; i<terms.size(); i++) {
        auto tS = s->terms[i];
        auto tR = terms[i];
        if (!tS.valid() || !tR.valid()) return false;
        if (tS.path.nodes.size() != tR.path.nodes.size()) return false;

        //cout << "var1 " << tR.var.value << " type: " << tR.var.concept << endl;
        //cout << "var2 " << tS.var.value << " type: " << tS.var.concept << endl;
        if (tR.var->concept != tS.var->concept) return false;
    }
    return true;
}

// TODO: parse concept statements here
Query::Query() {}

Query::Query(string q) {
    vector<string> parts = VRReasoner::split(q, ':');
    request = Statement::New(parts[0]);
    parts = VRReasoner::split(parts[1], ';');
    for (int i=0; i<parts.size(); i++) statements.push_back(Statement::New(parts[i], i));
}

string Query::toString() {
    string res = request->toString() + " :";
    for (auto s : statements) res += " " + s->toString();
    return res;
}

Term::Term(string s) : path(s), str(s) {;}

bool Term::valid() { return var->valid; }

bool Term::operator==(Term& other) {
    if (!valid() || !other.valid()) return false;

    for (auto i1 : var->instances) {
        vector<string> val1 = i1.second->getAtPath(path.nodes);
        for (auto i2 : other.var->instances) {
            vector<string> val2 = i2.second->getAtPath(other.path.nodes);
            for (string s2 : val1) {
                for (string s3 : val2) {
                    if (s2 == s3) return true;
                }
            }
        }
        for (string s : val1) if (s == other.var->value) return true;
    }
    return false;
}

/* TODO
 - non matching paths may be valid rule!

*/

bool VRReasoner::findRule(StatementPtr statement, Context& context) {
    cout << pre << "     search rule for " << statement->toString() << endl;
    for ( auto r : context.onto->getRules()) { // no match found -> check rules and initiate new queries
        if (!context.rules.count(r->rule)) continue;
        Query query = context.rules[r->rule];
        if (query.request->verb != statement->verb) continue; // rule verb does not match
        cout << pre << "      rule " << query.request->toString() << endl;

        //query.request.updateLocalVariables(context.vars, context.onto);
        if (!statement->match(query.request)) continue; // statements are not similar enough

        query.request = statement; // TODO: use pointer? new query needs to share ownership of the statement
        context.queries.push_back(query);
        cout << pre << yellowBeg << "      add query " << query.toString() << colEnd << endl;
        return true;
    }
    cout << pre << "      no rule found " << endl;
    return false;
}

bool VRReasoner::is(StatementPtr statement, Context& context) {
    auto& left = statement->terms[0];
    auto& right = statement->terms[1];
    if ( context.vars.count(left.var->value) == 0) return false; // check if context has a variable with the left value
    if (!left.valid() || !right.valid()) return false; // return if one of the sides invalid

    bool b = left == right;
    bool NOT = statement->verb_suffix == "not";
    cout << pre << "   " << left.str << " is " << (b?"":" not ") << (NOT?" not ":"") << right.var->value << endl;

    return ( (b && !NOT) || (!b && NOT) );
}

bool Variable::has(VariablePtr other, VROntologyPtr onto) {
    for (auto i1 : instances) { // all instances of that variable
        vector<VREntityPtr> matches;
        vector<VREntityPtr> toDiscard;
        for (auto i2 : other->instances) { // check each instance of the other variable

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

bool VRReasoner::has(StatementPtr statement, Context& context) { // TODO
    auto& left = statement->terms[0];
    auto& right = statement->terms[1];

    bool b = left.var->has(right.var, context.onto);
    cout << pre << "  " << left.str << " has " << (b?"":"not") << " " << right.str << endl;
    cout << "RES " << &right.var << "   " << right.var->toString() << endl;
    if (b) { statement->state = 1; return true; }

    // DEBUG -> TODO

    auto Cconcept = context.onto->getConcept( right.var->concept ); // child concept
    auto Pconcept = context.onto->getConcept( left.var->concept ); // parent concept
    if (Cconcept == 0) { cout << "Warning (has): first concept " << right.var->concept << " not found!\n"; return false; }
    if (Pconcept == 0) { cout << "Warning (has): second concept " << left.var->concept << " not found!\n"; return false; }
    auto prop = Pconcept->getProperties( Cconcept->getName() );
    if (prop.size() == 0) cout << "Warning: has evaluation failed, property " << right.var->value << " missing!\n"; return false;
    return false;
}

// apply the statement changes to world
bool VRReasoner::apply(StatementPtr statement, Context& context) {
    if (statement->verb == "is") {
        auto& left = statement->terms[0];
        auto& right = statement->terms[1];
        left.var->value = right.var->value; // TOCHECK
        statement->state = 1;
        cout << pre << greenBeg << "  set " << left.str << " to " << right.str << colEnd << endl;
    }

    if (statement->verb == "has") {
        auto& left = statement->terms[0];
        auto& right = statement->terms[1];
        auto Cconcept = context.onto->getConcept( right.var->concept ); // child concept
        auto Pconcept = context.onto->getConcept( left.var->concept ); // parent concept
        if (!Pconcept || !Cconcept) { cout << "Warning: failed to apply " << statement->toString() << endl; return false; }
        auto prop = Pconcept->getProperties( Cconcept->getName() );
        if (prop.size() == 0) { cout << "Warning: failed to apply " << statement->toString() << endl; return false; }
        for (auto i : left.var->instances) i.second->add(prop[0]->getName(), right.var->value); // TODO: the first parameter is wrong
        statement->state = 1;
        cout << pre << greenBeg << "  give " << right.str << " to " << left.str << colEnd << endl;
    }

    if (statement->verb == "q") {
        string v = statement->terms[0].var->value;
        cout << "RES2 " << &context.vars[v] << "   " << context.vars[v]->toString();
        if (context.results.count(v) == 0) context.results[v] = Result();
        for (auto i : context.vars[v]->instances) context.results[v].instances.push_back(i.second);
        statement->state = 1;
        cout << pre << greenBeg << "  add result " << v << colEnd << endl;
    }

    return true;
}

bool VRReasoner::evaluate(StatementPtr statement, Context& context) {
    cout << pre << " " << statement->place << " eval " << statement->toString() << endl;
    statement->updateLocalVariables(context.vars, context.onto);

    if (!statement->isSimpleVerb()) { // resolve (anonymous?) variables
        string var = statement->terms[0].path.root;
        context.vars[var] = Variable::create( context.onto, statement->verb, var );
        cout << pre << blueBeg << "  added variable " << context.vars[var]->toString() << colEnd << endl;
        statement->state = 1;
        return true;
    }

    if (statement->verb == "is") return is(statement, context);
    if (statement->verb == "has") return has(statement, context);
    return false;
}

void Query::checkState() {
    int r = 1;
    for (auto i : statements) if(i->state == 0) r = 0;
    request->state = r;
}
    // TODO: introduce requirements rules for the existence of some individuals

vector<Result> VRReasoner::process(string initial_query, VROntologyPtr onto) {
    cout << pre << initial_query << endl;

    Context context(onto); // create context
    context.queries.push_back(Query(initial_query));

    while( context.queries.size() ) { // while queries to process
        Query& query = context.queries.back();
        query.checkState();
        auto request = query.request;
        if (request->state == 1) {
            apply(request, context);
            cout << pre << redBeg << " solved: " << query.toString() << colEnd << endl;
            context.queries.pop_back(); continue;
        }; // query answered, pop and continue

        cout << pre << redBeg << "QUERY " << query.toString() << colEnd << endl;

        request->updateLocalVariables(context.vars, context.onto);

        for (auto& statement : query.statements) {
            if (statement->state == 1) continue;
            if (evaluate(statement, context)) {
                statement->state = 1;
            } else {
                if ( findRule(statement, context) ) continue;
                apply(statement, context);
            }
        }

        context.itr++;
        if (context.itr >= context.itr_max) break;
    }

    cout << pre << " break after " << context.itr << " iterations\n";
    for (auto r : context.results) {
        cout << pre << "  result " << r.first << endl;
        for (auto i : r.second.instances) cout << pre << "   instance " << i->toString() << endl;
    }

    vector<Result> res;
    for (auto r : context.results) res.push_back(r.second);
    return res;
}

VRReasonerPtr VRReasoner::create() { return VRReasonerPtr( new VRReasoner() ); }


