//
// Created by mitrikyle on 3/7/16.
//

#include <sstream>
#include "IntelWeb.h"
// going to need this
#include <queue>
#include <set>


IntelWeb::IntelWeb() {
    // TODO?
}

IntelWeb::~IntelWeb() {
    // TODO
}

bool IntelWeb::createNew(const std::string &filePrefix, unsigned int maxDataItems) {
    // make sure the maps are already closed
    close();
    std::string initiator_string = filePrefix;
    initiator_string.append("-initiator-map");
    std::string target_string = filePrefix;
    target_string.append("-target-map");
    // TODO don't just use maxDataItems, actually use the load factor
    if (m_initiator_map.createNew(initiator_string, maxDataItems) &&
            m_target_map.createNew(target_string,maxDataItems)){
        // if succesful
        return true;
    } else {
        // unsuccesful
        m_initiator_map.close();
        m_target_map.close();
        return false;
    }
}

bool IntelWeb::openExisting(const std::string &filePrefix) {
    // make sure the maps are already closed
    close();
    std::string initiator_string = filePrefix;
    initiator_string.append("-initiator-map");
    std::string target_string = filePrefix;
    target_string.append("-target-map");
    // try to open the files with the datas
   if ( m_initiator_map.openExisting(initiator_string) && m_target_map.openExisting(target_string)){
       return true;
   } else {
       // unsuccessful
       m_initiator_map.close();
       m_target_map.close();
       return false;
   }

}

void IntelWeb::close() {
    // close all our maps
    m_target_map.close();
    m_target_map.close();
}

bool IntelWeb::ingest(const std::string &telemetryFile) {
    ifstream file;
    std::string line;
    // open the file
    file.open("/home/mitrikyle/ClionProjects/project4/log.txt");
    // if succesful
    if(file.is_open()){
        // while we aren't at the end of the file
        while(getline(file,line)){
            // Things are parsed and stored in maps
            std::cout << line << endl;
            std::string buffer,machine,initiator,target;
            std::stringstream ss(line);
            // we know the first thign in the file is the machine
            // the second is the intiator
            // and 3rd is the target
            // THE DATA MUST ALWAYS BE IN THIS ORDER IN A VALID LOGFILE
            ss >> machine;
            ss >> initiator;
            ss >> target;
            cout << "Machine: " << machine << endl;
            cout << "Initiator: " << initiator << endl;
            cout << "Target: " << target << endl;
            // store this data in our maps.
            // unstable code below TODO WARNING
          //  m_initiator_map.insert(initiator, target, machine);
          //  m_target_map.insert(target,initiator,machine);

        }
        return true;
    }
    return false;
}

unsigned int IntelWeb::crawl(const std::vector <std::string> &indicators, unsigned int minPrevalenceToBeGood,
                             std::vector <std::string> &badEntitiesFound,
                             std::vector<InteractionTuple> &interactions) {
    // indicators is a vector of known malicous entities

    // Compute prevalence
    // prevalence = no. occcurences in target
    // + no. occurences in intiator
    // TODO
    // for every malicious thingy
    // add it to the queue
    // this queue stores all the bad entities :o
    std::queue<string> bad_entities;
    // this set stores all the bad entiteies we already dealt with
    std::set<string> bad_entities_history;
    for (std::string indicator : indicators){
        bad_entities.push(indicator);
    }
    while (!bad_entities.empty()){
        std::string curr = bad_entities.front();
        // if we are checking something in history, continue
        if (bad_entities_history.find(curr) != bad_entities_history.end())
            continue;
        // add the curr string to the history
        bad_entities_history.insert(curr);
        bad_entities.pop();
        // getiterator to all files associated with the current, first from the intiatior map
        auto it = m_initiator_map.search(curr);
        // TODO what about that other map :o
        // apply the rules to all the associated files
        while (it.isValid()){
            // if the value is already malicious, go to next
            if (bad_entities_history.find((*it).value) != bad_entities_history.end()){
                ++it;
                continue;
            } else {
                // otherwise apply the actual rules wtf
                // get the prevalance of the value/target
                int prevalance = 0;
                auto initiator_it = m_initiator_map.search(curr);
                while (initiator_it.isValid()){
                    ++prevalance;
                    ++initiator_it;
                }
                auto target_it = m_target_map.search(curr);
                while (target_it.isValid()){
                    ++prevalance;
                    ++target_it;
                }

                // we now, probably inefficiently , have the prevalance
                // if the prevalance is greater than equal to the good threshhold, continue,
                // because the thing is then not a virus
                if (prevalance >= minPrevalenceToBeGood){
                    ++it;
                    //TODO, also add to the history? or nah?
                    continue;
                } else {
                    // store new virus value
                    std::string bad_discovery = (*it).value;
                    // otherwise WE FOUND A VIRUS OMG WTF TO DO?
                    // add it to the vector of bad entities?
                    // TODO should values in the given vector of malicious values be added to badEntitiesFound as well???
                    badEntitiesFound.push_back(bad_discovery);
                    // add to the queue of bad entites
                    bad_entities.push(bad_discovery);
                    // add all the interaction pairs
                    // TODO DEAR LORD HOW MUCH WHILE LOOPS?
                    while (it.isValid()){
                        MultiMapTuple temp = (*it);
                        InteractionTuple interactionTuple(temp.key,temp.value,temp.context);
                        // store this tuple in the vector
                        interactions.push_back(interactionTuple);
                        // increment the iterator
                        ++it;
                    }


                }

            }

        }
    }
    // while the queue isn't empty,
    // apply rules to the item in queue
        // APPLYING RULES
        // basically rules say if two files have a relationship, and one is malicious,
        // then the other becomes malicious unless it has a high prevalcne
    // add the item tp the set of already done items
    // add all the newly discovered malicious items to the queue
    // keep going
    return 0;
}

bool IntelWeb::purge(const std::string &entity) {
    return false;
}
