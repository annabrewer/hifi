//
//  Created by Amer Cerkic 05/02/2019
//  Copyright 2019 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "LinuxPlatform.h"
#include "../PlatformKeys.h"

#include <thread>
#include <string>
#include <CPUIdent.h>
#include <GPUIdent.h>

using namespace platform;

void LinuxInstance::enumerateCpu() {
    json cpu = {};

    cpu[keys::cpu::vendor] = CPUIdent::Vendor();
    cpu[keys::cpu::model] = CPUIdent::Brand();
    cpu[keys::cpu::numCores] = std::thread::hardware_concurrency();

    _cpu.push_back(cpu);
}

void LinuxInstance::enumerateGpu() {
    GPUIdent* ident = GPUIdent::getInstance();
    json gpu = {};
    gpu[keys::gpu::vendor] = ident->getName().toUtf8().constData();
    gpu[keys::gpu::model] = ident->getName().toUtf8().constData();
    gpu[keys::gpu::videoMemory] = ident->getMemory();
    gpu[keys::gpu::driver] = ident->getDriver().toUtf8().constData();

    _gpu.push_back(gpu);
    _display = ident->getOutput();
}

void LinuxInstance::enumerateMemory() {
    json ram = {};
    ram[keys::memTotal]=0;

    _memory.push_back(ram);
}

void LinuxInstance::enumerateComputer(){
    
    _computer[keys::computer::OS] = keys::computer::OS_LINUX;
    _computer[keys::computer::vendor] = "";
    _computer[keys::computer::model] = "";
}

