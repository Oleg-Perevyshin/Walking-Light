// ИНФО

import {h, Component} from "preact";
import Message from "./Message";

export default class RFConfig extends Component {
    constructor() {
        super();
        this.setState({
            expanded: {
                info: false
            }
        });
    }

    componentWillMount() {
        this.context.sendWS("actualInfo");
    }

    render({}) {
        const {actualInfo} = this.context;
        const {
            actID, actDM, actDN, actV, actLV, actDU, actDP,
            actVCC, actStaSSID, actStaPSK, actWRSSI, actWCH, actStaIP, actStaMS, actStaGW, actStaMAC,
            actApSSID, actApPSK, actApIP, actApMS, actApGW, actApMAC,
            actDA, actRCH
        } = actualInfo;

        return (
            <div className="sb">
                <input type="checkbox" id="info" className="checker"
                       checked={this.state.expanded.info}
                       onChange={this.linkState("expanded.info")}/>
                <label for="info"><p className="tsb"><Message key="n.m.info.t"/></p></label>
                <div className="spl">
                    <div className="block-in">
                        <p className="bold"><Message key="n.m.info.m"/></p>
                        <p><Message key="n.m.info.device"/>{actDM} | {actDN}</p>
                        <p><Message key="n.m.info.di"/>{actID}</p>
                        <p><Message key="n.m.info.curver"/>{actV} | <Message key="n.m.info.latver"/>{actLV}</p>
                        <p><Message key="n.m.info.user"/>{actDU}</p>
                        <p><Message key="n.m.info.psk"/>{actDP}</p>
                    </div>
                    <div className="block-in">
                        <p className="bold"><Message key="n.m.info.sta"/></p>
                        <p><Message key="n.m.info.vcc"/>{actVCC} mV</p>
                        <p><Message key="n.m.info.ssid"/>{actStaSSID}</p>
                        <p><Message key="n.m.info.psk"/>{actStaPSK}</p>
                        <p><Message key="n.m.info.rssi"/>{actWRSSI} | <Message key="n.m.info.ch"/>{actWCH}</p>
                        <p><Message key="n.m.info.ip"/>{actStaIP}</p>
                        <p><Message key="n.m.info.mask"/>{actStaMS}</p>
                        <p><Message key="n.m.info.gateway"/>{actStaGW}</p>
                        <p><Message key="n.m.info.mac"/>{actStaMAC}</p>
                        <p className="bold"><Message key="n.m.info.ap"/></p>
                        <p><Message key="n.m.info.ssid"/>{actApSSID}</p>
                        <p><Message key="n.m.info.psk"/>{actApPSK}</p>
                        <p><Message key="n.m.info.ip"/>{actApIP}</p>
                        <p><Message key="n.m.info.mask"/>{actApMS}</p>
                        <p><Message key="n.m.info.gateway"/>{actApGW}</p>
                        <p><Message key="n.m.info.mac"/>{actApMAC}</p>
                    </div>
                    <div className="block-in">
                        <p className="bold"><Message key="n.m.info.radio"/></p>
                        <p><Message key="n.m.info.da"/>{actDA}</p>
                        <p><Message key="n.m.info.radio_channel"/>{actRCH}</p>
                    </div>
                </div>
            </div>
        );
    }
}