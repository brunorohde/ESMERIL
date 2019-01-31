/*==============================================================================
	ofelia: OpenFrameworks as an External Library for Interactive Applications

	Copyright (c) 2018 Zack Lee <cuinjune@gmail.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.

	See https://github.com/cuinjune/ofxOfelia for documentation
 ==============================================================================*/

#include "ofApp.h"
#include "ofeliaSetup.h"
#include "ofeliaWindow.h"
#include "ofxAccelerometer.h"

// ESMERIL MOD
#include "ofxZipPass.h"

#define AUTO_NUM_CHANNELS -1

// ESMERIL MOD - Declara externals
extern "C" {
    extern void freeverb_tilde_setup(void);
    extern void limiter_tilde_setup(void);
    extern void z_tilde_setup(void);
}

//--------------------------------------------------------------
void ofApp::initAudio()
{
    // you can change the settings below
    sampleRate = 44100;
    numInputChannels = AUTO_NUM_CHANNELS;
    numOutputChannels = AUTO_NUM_CHANNELS;

    // the number of libpd ticks per buffer,
    // used to compute the audio buffer len: tpb * blocksize (always 64)
    ticksPerBuffer = 8; // 8 * 64 = buffer len of 512
    shouldReinitAudio = false;
}

//--------------------------------------------------------------
void ofApp::setup()
{
    ofBackground(0,0,0,255);
    accelForce.set(0.0f, 0.0f, -1.0f);
    ofxAccelerometer.setup();
    initAudio();
    setupAudio(false);
    setupMidi();
    pd.openPatch("pd/main.pd");

    //ESMERIL MOD
	ofRegisterURLNotification(this);        // Faz registro para poder obter notificações URL durante downloads
}

//--------------------------------------------------------------
void ofApp::update()
{
    if (shouldReinitAudio) {

        setupAudio(true); //reinit audio
        shouldReinitAudio = false;
    }

    /*
    //detect device orientation change using accelerometer (Android)
    ofVec3f accelReal = ofxAccelerometer.getRawAcceleration();
    accelReal.normalize();
    const float lerpFactor = 0.1f;
    accelForce += (accelReal - accelForce) * lerpFactor;
    const int inclination = static_cast<int>(round(ofRadToDeg(acos(accelForce.z))));

    if (inclination > 45 && inclination < 135) {

        const int rotation = static_cast<int>(round(ofRadToDeg(atan2(accelForce.x, accelForce.y))));

        if (!ofeliaWindow::isDefOrienLandscape) {

            if ((rotation > 175 && rotation < 185) ||
                (rotation > -185 && rotation < -175))
                deviceOrientationChanged(OF_ORIENTATION_DEFAULT);
            else if (rotation > 85 && rotation < 95)
                deviceOrientationChanged(OF_ORIENTATION_90_RIGHT);
            else if (rotation > -5 && rotation < 5)
                deviceOrientationChanged(OF_ORIENTATION_180);
            else if (rotation > -95 && rotation < -85)
                deviceOrientationChanged(OF_ORIENTATION_90_LEFT);
        }
        else {

            if ((rotation > 175 && rotation < 185) ||
                (rotation > -185 && rotation < -175))
                deviceOrientationChanged(OF_ORIENTATION_90_RIGHT);
            else if (rotation > 85 && rotation < 95)
                deviceOrientationChanged(OF_ORIENTATION_180);
            else if (rotation > -5 && rotation < 5)
                deviceOrientationChanged(OF_ORIENTATION_90_LEFT);
            else if (rotation > -95 && rotation < -85)
                deviceOrientationChanged(OF_ORIENTATION_DEFAULT);
        }
    }
    */
}

//--------------------------------------------------------------
void ofApp::draw()
{
}

void ofApp::exit()
{
    // clean up midi inputs and outputs
    for (int i=0; i<9; ++i) {

        midiIns[i].closePort();
        midiIns[i].removeListener(this);
    }
    for (int i=0; i<9; ++i)
        midiOuts[i].closePort();

    // clean up audio inputs and outputs
    stream.close();

    // exit accelerometer
    ofxAccelerometer.exit();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
}

//--------------------------------------------------------------
void ofApp::touchDown(int x, int y, int id)
{
}

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id)
{
}

//--------------------------------------------------------------
void ofApp::touchUp(int x, int y, int id)
{
}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(int x, int y, int id)
{
}

//--------------------------------------------------------------
void ofApp::touchCancelled(int x, int y, int id)
{
}

//--------------------------------------------------------------
void ofApp::swipe(ofxAndroidSwipeDir swipeDir, int id)
{
}

//--------------------------------------------------------------
void ofApp::pause()
{
    ofeliaWindow::sendFocusToPd(0);
}

//--------------------------------------------------------------
void ofApp::stop()
{
    // clean up midi inputs and outputs
    for (int i=0; i<9; ++i) {

        midiIns[i].closePort();
        midiIns[i].removeListener(this);
    }
    for (int i=0; i<9; ++i)
        midiOuts[i].closePort();

    // clean up audio inputs and outputs
    stream.close();

    // exit accelerometer
    ofxAccelerometer.exit();
}

//--------------------------------------------------------------
void ofApp::resume()
{
    ofeliaWindow::sendFocusToPd(1);
}

//--------------------------------------------------------------
void ofApp::reloadTextures()
{
}

//--------------------------------------------------------------
bool ofApp::backPressed()
{
    ofeliaWindow::sendBackToPd();
	return true; //setting this to false will make the app quit immediately when back button is pressed
}

//--------------------------------------------------------------
void ofApp::okPressed()
{
}

//--------------------------------------------------------------
void ofApp::cancelPressed()
{
}

//--------------------------------------------------------------
/*
void ofApp::deviceOrientationChanged(int newOrientation)
{
    if (newOrientation == OF_ORIENTATION_UNKNOWN ||
        newOrientation == orientation) //ignore unknown or repeated orientations
        return;
    orientation = newOrientation;

    if (t_ofeliaWindow::orienChangedSym->s_thing) {

        int orien = -1;

        if (!ofeliaWindow::isDefOrienLandscape) {

            switch (newOrientation) {

                case OF_ORIENTATION_DEFAULT:
                    orien = ORIENTATION_DEFAULT;
                    break;
                case OF_ORIENTATION_90_RIGHT:
                    orien = ORIENTATION_RIGHT;
                    break;
                case OF_ORIENTATION_180:
                    orien = ORIENTATION_INVERT;
                    break;
                case OF_ORIENTATION_90_LEFT:
                    orien = ORIENTATION_LEFT;
                    break;
                default:
                    break;
            }
        }
        else {

            switch (newOrientation) {

                case OF_ORIENTATION_90_RIGHT:
                    orien = ORIENTATION_DEFAULT;
                    break;
                case OF_ORIENTATION_180:
                    orien = ORIENTATION_RIGHT;
                    break;
                case OF_ORIENTATION_90_LEFT:
                    orien = ORIENTATION_INVERT;
                    break;
                case OF_ORIENTATION_DEFAULT:
                    orien = ORIENTATION_LEFT;
                    break;
                default:
                    break;
            }
        }

        if (orien != -1) {

            t_atom av[1];
            av[0].a_type = A_FLOAT;
            av[0].a_w.w_float = static_cast<t_float>(orien);
            typedmess(t_ofeliaWindow::orienChangedSym->s_thing, gensym("orien"), 1, av);
        }
    }
}
*/

//--------------------------------------------------------------
void ofApp::audioReceived(float *input, int bufferSize, int nChannels)
{
    pd.audioIn(input, bufferSize, nChannels);
}

//--------------------------------------------------------------
void ofApp::audioRequested(float *output, int bufferSize, int nChannels)
{
    pd.audioOut(output, bufferSize, nChannels);
}

//--------------------------------------------------------------
void ofApp::print(const std::string& message) {
    ofLogNotice() << message;
}

//--------------------------------------------------------------

// ESMERIL MOD

void ofApp::receiveBang(const std::string& dest)
{
    // Imprime mensagens recebidas do Pd de [send]'s inscritos
    //ofLogNotice() << dest;

    // Método getSize para obter tamanho do arquivo durante download e comparar com tamanho total para obter progresso
    if (dest == "getSize"){
        isOpen = file.open(downFile);               // Abre arquivo em download numa instância ofFile para acompanhar tamanho e retorna um bool para a variável isOpen, que permite perguntar tamanho do arquivo
        if (isOpen) {                               // Se conseguir abrir arquivo .zip permite consultar tamanho
            float size = (file.getSize());          // Obtém tamanho do arquivo em bytes
            pd.sendFloat("getSizeFB", size);        // Envia tamanho em bytes (float) para Pd
            file.close();                           // Fecha o arquivo na instância do ofFile criada para acompanhar o download
            isOpen = false;                         // Envia sinal para bloquear leitura do tamanho do arquivo
        }
    } else if (dest == "downloadClear") {
        ofRemoveAllURLRequests();
        //ofRemoveURLRequest(downloadID);
        //ofStopURLLoader();
        ofLogNotice() << "RODANDO MÉTODO CLEAR";
    }
}

void ofApp::receiveFloat(const std::string& dest, float value)
{
}

void ofApp::receiveSymbol(const std::string& dest, const std::string& symbol)
{
}

void ofApp::receiveList(const std::string& dest, const List& list)
{

    // Imprime mensagens recebidas do Pd de [send]'s inscritos
    //ofLogNotice() << dest;
    //ofLogNotice() << list.toString();

    // Desempacota arquivo .zip para um caminho especificado pelo Pd

    if (dest == "unzip"){
        ofxUnzipPass zip(list.getSymbol(0), "");                // Cria objeto ofxUnzipPass com primeiro item da lista obtida do Pd e com password vazio
	    if (zip.isOk()) {
	        bool success = zip.unzipTo(list.getSymbol(1));      // Descompacta arquivo com método unzipTo e retorna bool sobre resultado
	        if (!success) {
                pd.sendFloat("unzipFB", 1);     // Se processo falha manda 1 para Pd
            }else if (success){
                pd.sendFloat("unzipFB", 2);     // Se for bem sucedido manda 2 para Pd
            }
	    }
	    else pd.sendFloat("unzipFB", 0);        // Se arquivo .zip não for aberto manda 0 para Pd
    }

    // Faz download de um arquivo em servidor para um caminho especificado pelo Pd

    else if (dest == "download") {
        downloadID = ofSaveURLAsync(list.getSymbol(0), list.getSymbol(1));  // downloadID é um int retornado usado para comparar ID das respostas URL abaixo
        downFile = list.getSymbol(1);
    }
}

// Método para receber resposta URL sobre o download

void ofApp::urlResponse(ofHttpResponse &httpResponse){
    if(httpResponse.request.getID() == downloadID){         // Checa ID do processo da resposta pra ver se é do downloadID
        int status = httpResponse.status;                   // Armazena status num int
        pd.sendFloat("downloadFB", status);                 // Envia status pro Pd
    }
}
// ESMERIL MOD END

void ofApp::receiveMessage(const std::string& dest, const std::string& msg, const List& list)
{
}

//------------------MIDI MESSAGES-------------------------------
void ofApp::receiveNoteOn(const int channel, const int pitch, const int velocity) {

    for (int i=0; i<9; ++i) {

        if (midiOuts[i].isOpen())
            midiOuts[i].sendNoteOn(channel, pitch, velocity);
    }
}

//--------------------------------------------------------------
void ofApp::receiveControlChange(const int channel, const int controller, const int value) {

    for (int i=0; i<9; ++i) {

        if (midiOuts[i].isOpen())
            midiOuts[i].sendControlChange(channel, controller, value);
    }
}

//--------------------------------------------------------------
// note: pgm nums are 1-128 to match pd
void ofApp::receiveProgramChange(const int channel, const int value) {

    for (int i=0; i<9; ++i) {

        if (midiOuts[i].isOpen())
            midiOuts[i].sendProgramChange(channel, value);
    }
}

//--------------------------------------------------------------
void ofApp::receivePitchBend(const int channel, const int value) {

    for (int i=0; i<9; ++i) {

        if (midiOuts[i].isOpen())
            midiOuts[i].sendPitchBend(channel, value);
    }
}

//--------------------------------------------------------------
void ofApp::receiveAftertouch(const int channel, const int value) {

    for (int i=0; i<9; ++i) {

        if (midiOuts[i].isOpen())
            midiOuts[i].sendAftertouch(channel, value);
    }
}

//--------------------------------------------------------------
void ofApp::receivePolyAftertouch(const int channel, const int pitch, const int value) {

    for (int i=0; i<9; ++i) {

        if (midiOuts[i].isOpen())
            midiOuts[i].sendPolyAftertouch(channel, pitch, value);
    }
}

//--------------------------------------------------------------
// note: pd adds +2 to the port num, so sending to port 3 in pd to [midiout],
//       shows up at port 1 in ofxPd
void ofApp::receiveMidiByte(const int port, const int byte) {

    for (int i=0; i<9; ++i) {

        if (midiOuts[i].isOpen())
            midiOuts[i].sendMidiByte(byte);
    }
}
//--------------------------------------------------------------
void ofApp::addMessage(string msg) {

    messageMutex.lock();
    ofLogNotice() << msg;
    messages.push_back(msg);
    while(messages.size() > maxMessages)
        messages.pop_front();
    messageMutex.unlock();
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {

    addMessage(msg.toString());
    midiMessage = msg;
    midiChan = midiMessage.channel;

    if (midiMessage.getStatusString(midiMessage.status) == "Note On") {

        pd.sendNoteOn(midiChan, midiMessage.pitch, midiMessage.velocity);
    }
    else if (midiMessage.getStatusString(midiMessage.status) == "Control Change") {

        pd.sendControlChange(midiChan, midiMessage.control, midiMessage.value);
    }
    else if (midiMessage.getStatusString(midiMessage.status) == "Program Change") {

        pd.sendProgramChange(midiChan, midiMessage.value); // note: pgm num range is 1 - 128
    }
    else if (midiMessage.getStatusString(midiMessage.status) == "Pitch Bend") {

        pd.sendPitchBend(midiChan, midiMessage.value - 8192); //note: ofxPd uses -8192 - 8192 while
        // [bendin] returns 0 - 16383,
        // so sending a val of 2000 gives 10192 in pd
    }
    else if (midiMessage.getStatusString(midiMessage.status) == "Aftertouch") {

        pd.sendAftertouch(midiChan, midiMessage.value);
    }
    else if (midiMessage.getStatusString(midiMessage.status) == "Poly Aftertouch") {

        pd.sendPolyAftertouch(midiChan, midiMessage.pitch, midiMessage.value);
    }
    else if (midiMessage.getStatusString(midiMessage.status) == "Sysex") {

        pd.sendSysex(midiMessage.portNum, midiMessage.value);       // note: pd adds +2 to the port number from
        pd.sendSysRealTime(midiMessage.portNum, midiMessage.value); // [midiin], [sysexin], & [realtimein].
        pd.sendMidiByte(midiMessage.portNum, midiMessage.value);    // so sending to port 0 gives port 2 in pd
    }
}

//--------------------------------------------------------------
void ofApp::midiInputAdded(string name, bool isNetwork)
{
    stringstream msg;
    msg << "ofxMidi: input added: " << name << " network: " << isNetwork;
    addMessage(msg.str());

    for (int i=0; i<9; ++i) {

        if (!midiIns[i].isOpen()) {

            midiIns[i].openPort(name);
            break;
        }
    }
}

//--------------------------------------------------------------
void ofApp::midiInputRemoved(string name, bool isNetwork)
{
    stringstream msg;
    msg << "ofxMidi: input removed: " << name << " network: " << isNetwork << endl;
    addMessage(msg.str());

    for (int i=0; i<9; ++i) {

        if (midiIns[i].isOpen() && midiIns[i].getName() == name) {

            midiIns[i].closePort();
            break;
        }
    }
}

//--------------------------------------------------------------
void ofApp::midiOutputAdded(string name, bool isNetwork)
{
    stringstream msg;
    msg << "ofxMidi: output added: " << name << " network: " << isNetwork << endl;
    addMessage(msg.str());

    for (int i=0; i<9; ++i) {

        if (!midiOuts[i].isOpen()) {

            midiOuts[i].openPort(name);
            break;
        }
    }
}

//--------------------------------------------------------------
void ofApp::midiOutputRemoved(string name, bool isNetwork)
{
    stringstream msg;
    msg << "ofxMidi: output removed: " << name << " network: " << isNetwork << endl;
    addMessage(msg.str());

    for (int i=0; i<9; ++i) {

        if (midiOuts[i].isOpen() && midiOuts[i].getName() == name) {

            midiOuts[i].closePort();
            break;
        }
    }
}

void ofApp::setupAudio(bool isReinit)
{
    // setup OF sound stream
    if (isReinit) {

        stream.close();
    }

    // sort device list into input & output
    vector<ofSoundDevice> inputDeviceList, outputDeviceList;
    getInputOutputAudioDeviceList(inputDeviceList, outputDeviceList);

    if (outputDeviceList.empty()) { // if no audio output device found, exit the app

        ofLogError() << "error : audio device not found";
        OF_EXIT_APP(1);
    }
    if (!isReinit) {

        inputDeviceID = outputDeviceID = -1;

        // search default input and output then assign sorted deviceID (not real)
        for (size_t i=0; i<inputDeviceList.size(); ++i) {

            if (inputDeviceList[i].isDefaultInput) {

                inputDeviceID = static_cast<int>(i);
                break;
            }
        }
        for (size_t i=0; i<outputDeviceList.size(); ++i) {

            if (outputDeviceList[i].isDefaultOutput) {

                outputDeviceID = static_cast<int>(i);
                break;
            }
        }

        // if no default input or output are found, assign the first deviceID
        if (inputDeviceID < 0)
            inputDeviceID = 0;

        if (outputDeviceID < 0)
            outputDeviceID = 0;
    }
    const vector<ofSoundDevice> &deviceList = getDeviceList();
    inputDeviceID = getClampIntValue(inputDeviceID, 0, static_cast<int>(inputDeviceList.size())-1);
    outputDeviceID = getClampIntValue(outputDeviceID, 0, static_cast<int>(outputDeviceList.size())-1);

    // convert sorted deviceID into real deviceID
    const int realInputDeviceID = inputDeviceList[static_cast<size_t>(inputDeviceID)].deviceID;
    const int realOutputDeviceID = outputDeviceList[static_cast<size_t>(outputDeviceID)].deviceID;

    if (!isReinit) {

        // use all available channels
        if (numInputChannels < 0)
            numInputChannels = deviceList[static_cast<size_t>(realInputDeviceID)].inputChannels;

        if (numOutputChannels < 0)
            numOutputChannels = deviceList[static_cast<size_t>(realOutputDeviceID)].outputChannels;
    }
    numInputChannels = getClampIntValue(numInputChannels, 0,
                                        deviceList[static_cast<size_t>(realInputDeviceID)].inputChannels);
    numOutputChannels = getClampIntValue(numOutputChannels, 0,
                                         deviceList[static_cast<size_t>(realOutputDeviceID)].outputChannels);
    const int realSampleRate = sampleRate;
    stream.setDevice(getDeviceList()[static_cast<size_t>(realOutputDeviceID)]);
    stream.setup(this, numOutputChannels, numInputChannels, realSampleRate,
                  ofxPd::blockSize()*ticksPerBuffer, 3);

    // setup Pd
    if(!pd.init(numOutputChannels, numInputChannels, realSampleRate, ticksPerBuffer-1, false)) {

        OF_EXIT_APP(1);
    }
    if (!isReinit) {

        // load libs
        ofelia_setup();
        freeverb_tilde_setup();
        limiter_tilde_setup();
        z_tilde_setup();

        // ESMERIL MOD
        pd.subscribe("unzip");          // Registra [send]'s a receber do Pd
        pd.subscribe("download");
        pd.subscribe("downloadClear");
        pd.subscribe("getSize");

        // add message receiver, required if you want to recieve messages
        pd.addReceiver(*this); // automatically receives from all subscribed sources

        // add midi receiver, required if you want to recieve midi messages
        pd.addMidiReceiver(*this); // automatically receives from all channels
    }
    else {

        ofSleepMillis(100);
    }
    pd.start();
}

void ofApp::setupMidi()
{
    // enables the network midi session between iOS and Mac OSX on a
    // local wifi network
    //
    // in ofxMidi: open the input/outport network ports named "Session 1"
    //
    // on OSX: use the Audio MIDI Setup Utility to connect to the iOS device
    //
    ofxMidi::enableNetworking();

    // set this class to receieve midi device (dis)connection events
    ofxMidi::setConnectionListener(this);

    // setup midiIns and midiOuts
    for (int i=0; i<9; ++i) {

        midiIns[i].ignoreTypes(false, false, false);
        midiIns[i].addListener(this);
    }
}

vector<ofSoundDevice> ofApp::getDeviceList() //get fake device list
{
    ofSoundDevice device;
    device.deviceID = 0;
    device.name = "Android Audio";
    device.inputChannels = 1;
    device.outputChannels = 2;
    device.isDefaultInput = true;
    device.isDefaultOutput = true;
    return {device};
}

void ofApp::getInputOutputAudioDeviceList(vector<ofSoundDevice> &inputDeviceList,
                                          vector<ofSoundDevice> &outputDeviceList)
{
    const vector<ofSoundDevice> &deviceList = getDeviceList();

    for (size_t i=0; i<deviceList.size(); ++i) {

        if (deviceList[i].inputChannels)
            inputDeviceList.push_back(deviceList[i]);

        if (deviceList[i].outputChannels)
            outputDeviceList.push_back(deviceList[i]);
    }
}
