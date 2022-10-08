// WIFI МОДУЛЬ

import {h, Component} from "preact";
import Message from "./Message";
import Button from "./Button";
import Select from "./Select";
import Input from "./Input";
import Loading from "./Loading";

export default class RFConfig extends Component {
    constructor() {
        super();
        this.setState({
            expanded: {
                wifi: true,
                ap: false,
                ap_adv: false
            },
            manualStaSSID: true,
            scanInProgress: true
        });
    }

    componentWillMount() {
        this.onStaSsidChange = this.onStaSsidChange.bind(this);
        this.onSTAIPModeChange = this.onSTAIPModeChange.bind(this);
        this.scanWiFi(true);
        this.scanWiFi = this.scanWiFi.bind(this);
    }

    onStaSsidChange(event) {
        if (event.target.value === '') {
            this.setState({manualStaSSID: true});
        } else {
            this.setState({manualStaSSID: false});
            this.context.setParam('staSSID', event.target.value);
        }
    }

    onSTAIPModeChange(event) {
        this.context.setParam('staIPMode', event.target.value === "1" ? 1 : 0);
    }

    scanWiFi(forceOldSSID = false) {
        this.setState({scanInProgress: true});
        this.context.sendWS("wifiScan");
        setTimeout(() => {
            forceOldSSID = forceOldSSID === true;
            let found = false;
            if (forceOldSSID && this.context.device.staSSID.length) {
                for (let net of this.context.networks)
                    if (net.ssid === this.context.device.staSSID)
                        found = true;
            } else if (this.context.networks.length) {
                this.context.device.staSSID = this.context.networks[0].ssid;
                found = true;
            }
            this.setState({manualStaSSID: !found});
            this.setState({scanInProgress: false});
        }, 1000);
    }

    render({}) {
        const {device, linkState, networks} = this.context;
        const {staSSID, staPSK, staIPMode, staIP, staMS, staGW, apSSID, apPSK, apIP, apMS, apGW} = device;

        return (
            <div className="sb">
                {/* РЕЖИМ "КЛИЕНТ" */}
                <input type="checkbox" id="wifi" className="checker"
                       checked={this.state.expanded.wifi}
                       onChange={this.linkState('expanded.wifi')}/>
                <label for="wifi"><p className="tsb"><Message key="n.m.wifi.t"/></p></label>
                <div className="spl">
                    {this.state.scanInProgress ? <Loading/> : (
                        <div>
                            <p className="bold"><Message key="n.m.wifi.sta.ssid"/></p>
                            <Select id="staSSIDSelect"
                                    className={"select w77 large" + (this.state.manualStaSSID ? ' hidden' : '')}
                                    onChange={this.onStaSsidChange} value={staSSID}>
                                {networks.map(network =>
                                    <option
                                        value={network.ssid}>{`${network.ssid} (${network.rssi}dBm, Ch${network.ch})`}</option>
                                )}
                                <option disabled/>
                                <option value=""><Message key="n.m.wifi.sta.hidden"/></option>
                            </Select>
                            <Input id="staSSID" type="text"
                                   className={"input w75 large" + (this.state.manualStaSSID ? '' : ' hidden')}
                                   onInput={linkState('device.staSSID')} value={staSSID}/>

                            {/* Пароль */}
                            <p className="bold"><Message key="n.m.wifi.sta.psk"/></p>
                            <Input type="text" className="input w75 large" onInput={linkState('device.staPSK')}
                                   value={staPSK}/>

                            {/* Параметры сети */}
                            <p className="bold"><Message key="n.m.wifi.sta.ip_conf"/></p>
                            <label><input name="staIPMode" type="radio" value="0" checked={!staIPMode}
                                          onChange={this.onSTAIPModeChange}/>
                                <Message key="n.m.ip.dhcp"/>
                            </label>
                            <label><input name="staIPMode" type="radio" value="1" checked={staIPMode}
                                          onChange={this.onSTAIPModeChange}/>
                                <Message key="n.m.ip.static"/>
                            </label>
                            <div className={(staIPMode ? 'adv-param' : ' hidden')}>
                                <p><Message key="n.m.ip.address"/></p>
                                <Input className="input w50 large" value={staIP} type="text"
                                       onInput={linkState('device.staIP')}/>
                                <p><Message key="n.m.ip.netmask"/></p>
                                <Input className="input w50 large" value={staMS} type="text"
                                       onInput={linkState('device.staMS')}/>
                                <p><Message key="n.m.ip.gateway"/></p>
                                <Input className="input w50 large" value={staGW} type="text"
                                       onInput={linkState('device.staGW')}/>
                            </div>
                            <br/>
                            <Button className="button w50 blue large" onClick={() => this.context.sendWS("setSTA")}>
                                <Message key="a.save"/>
                            </Button>
                            <br/>
                            {/* Синхронизация параметров авторизации */}
                            <p className="bold"><Message key="n.m.wifi.synch"/></p>
                            <Button className="button w50 blue large" onClick={() => this.context.sendWS("synch")}><Message
                                key="a.perform"/></Button>
                        </div>
                    )}
                    <br/>
                    {/* РЕЖИМ "ТОЧКА ДОСТУПА" */}
                    <input type="checkbox" id="ap" className="checker"
                           checked={this.state.expanded.ap}
                           onChange={this.linkState('expanded.ap')}/>
                    <label for="ap"><h2 className="bold"><Message key="n.m.wifi.ap.t"/></h2>
                    </label>
                    <div className="spl">
                        {/* Имя модуля в режиме "Точка Доступа" */}
                        <p className="bold"><Message key="n.m.wifi.ap.ssid"/></p>
                        <Input type="text" maxlength="16" className="input w75 large" value={apSSID}
                               onInput={linkState('device.apSSID')}/>
                        {/* Пароль */}
                        <p className="bold"><Message key="n.m.wifi.ap.psk"/></p>
                        <Input type="text" maxlength="16" className="input w75 large" value={apPSK}
                               onInput={linkState('device.apPSK')}/>
                        <br/>
                        {/* Статические параметры сети */}
                        <p className="bold"><Message key="n.m.wifi.ap.ip_conf"/></p>
                        <input type="checkbox" id="ap_adv" className="checker"/>
                        <label for="ap_adv" className="toggle"/>
                        <div className="spl">
                            <div className="adv-param">
                                <input type="checkbox" id="AP" className="checker"/>
                                <p><Message key="n.m.ip.address"/></p>
                                <Input className="input w50 large" value={apIP} type="text"
                                       onInput={linkState('device.apIP')}/>
                                <p><Message key="n.m.ip.netmask"/></p>
                                <Input className="input w50 large" value={apMS} type="text"
                                       onInput={linkState('device.apMS')}/>
                                <p><Message key="n.m.ip.gateway"/></p>
                                <Input className="input w50 large" value={apGW} type="text"
                                       onInput={linkState('device.apGW')}/>
                            </div>
                        </div>
                        <br/>
                        <Button className="button w75 blue large" onClick={() => this.context.sendWS("setAP")}><Message
                            key="a.save"/></Button>
                    </div>
                </div>
            </div>
        )
    }
}