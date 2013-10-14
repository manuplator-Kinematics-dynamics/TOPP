// -*- coding: utf-8 -*-
// Copyright (C) 2013 Quang-Cuong Pham <cuong.pham@normalesup.org>
//
// This file is part of the Time-Optimal Path Parameterization (TOPP) library.
// TOPP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#include "TOPP.h"
#include "KinematicLimits.h"
#include "TorqueLimits.h"


#include <boost/python.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

using namespace TOPP;

class TOPPInstance {
public:
    TOPPInstance(std::string problemtype, std::string constraintsstring,std::string trajectorystring,std::string tuningsstring){
        if(problemtype.compare("KinematicLimits")==0) {
            pconstraints = new KinematicLimits(constraintsstring);
        }
        else if(problemtype.compare("TorqueLimits")==0) {
            pconstraints = new TorqueLimits(constraintsstring);
        }
        else if(problemtype.compare("QuadraticConstraints")==0) {
            pconstraints = new QuadraticConstraints(constraintsstring);
        }
        ptrajectory = new Trajectory(trajectorystring);
        tunings = Tunings(tuningsstring);
    }

    Constraints* pconstraints;
    Trajectory* ptrajectory;
    Trajectory restrajectory;

    Tunings tunings;
    std::string restrajectorystring;
    std::string resprofilesliststring;
    std::string switchpointsliststring;
    dReal resduration;
    dReal sdendmin,sdendmax;



    int RunComputeProfiles(dReal sdbeg, dReal sdend){
        int res = ComputeProfiles(*pconstraints,*ptrajectory,tunings,sdbeg,sdend);
        resduration = pconstraints->resduration;
        return res;
    }


    void ReparameterizeTrajectory(){
        ptrajectory->Reparameterize(pconstraints->resprofileslist,tunings.reparamtimestep,restrajectory);
    }


    void RunVIP(dReal sdbegmin, dReal sdbegmax){
        int ret = VIP(*pconstraints,*ptrajectory,tunings,sdbegmin,sdbegmax,sdendmin,sdendmax);
        if(ret == 0) {
            sdendmin = -1;
            sdendmax = -1;
        }
    }

    void WriteResultTrajectory(){
        std::stringstream ss;
        restrajectory.Write(ss);
        restrajectorystring = ss.str();
    }

    void WriteProfilesList(){
        std::list<Profile>::iterator itprofile = pconstraints->resprofileslist.begin();
        std::stringstream ss;
        pconstraints->WriteMVCBobrow(ss);
        ss << "\n";
        pconstraints->WriteMVCCombined(ss);
        ss << "\n";
        while(itprofile!=pconstraints->resprofileslist.end()) {
            itprofile->Write(ss);
            ss << "\n";
            itprofile++;
        }
        resprofilesliststring = ss.str();
    }

    void WriteSwitchPointsList(){
        std::stringstream ss;
        std::list<SwitchPoint>::iterator itsw = pconstraints->switchpointslist.begin();
        while(itsw != pconstraints->switchpointslist.end()) {
            ss << itsw->s << " " << itsw->sd << " " << itsw->switchpointtype << "\n";
            itsw++;
        }
        switchpointsliststring = ss.str();
    }
};





BOOST_PYTHON_MODULE(TOPPbindings)
{
    using namespace boost::python;
    class_<TOPPInstance>("TOPPInstance", init<std::string,std::string,std::string,std::string>())
    .def_readonly("restrajectorystring", &TOPPInstance::restrajectorystring)
    .def_readonly("resprofilesliststring", &TOPPInstance::resprofilesliststring)
    .def_readonly("switchpointsliststring", &TOPPInstance::switchpointsliststring)
    .def_readonly("resduration", &TOPPInstance::resduration)
    .def_readonly("sdendmin", &TOPPInstance::sdendmin)
    .def_readonly("sdendmax", &TOPPInstance::sdendmax)
    .def("RunComputeProfiles",&TOPPInstance::RunComputeProfiles)
    .def("ReparameterizeTrajectory",&TOPPInstance::ReparameterizeTrajectory)
    .def("RunVIP",&TOPPInstance::RunVIP)
    .def("WriteResultTrajectory",&TOPPInstance::WriteResultTrajectory)
    .def("WriteProfilesList",&TOPPInstance::WriteProfilesList)
    .def("WriteSwitchPointsList",&TOPPInstance::WriteSwitchPointsList);

}