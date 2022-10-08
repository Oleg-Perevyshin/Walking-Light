// ИДЕНТИФИКАЦИЯ

import {h, Component} from "preact";
import Message from "./Message";
import Button from "./Button";
import Input from "./Input";
import {stringify} from "querystring";

export default class IDConfig extends Component {
    constructor() {
        super();
        this.setState({
            colors: ['d15', 'f39', 'f72', 'fc1', '1b0', '5ec', '2bf', '27f', '92f', '999'],
            expanded: {
                id: false
            }
        });
    }


    render({}, { colors }) {
        const { device, linkState, setParam } = this.context;
        const {dn, du, dp} = device;

        return (
            <div className="sb">
                <input type="checkbox" id="id" className="checker"
                       checked={this.state.expanded.id}
                       onChange={this.linkState('expanded.id')}/>
                <label for="id"><p className="tsb"><Message key="n.m.id.t"/></p></label>
                <div className="spl">
                    {/* Имя устройства */}
                    <p className="bold"><Message key="n.m.id.dn"/></p>
                    <Input className="input w75 large" value={dn} type="text" maxlength="16"
                           onInput={linkState('device.dn')}/>

                    {/* Выбор цветовой зоны */}
                    <p className="bold"><Message key="n.m.id.color"/></p>
                    {colors.map(color =>
                        <a className={"cDev" + (device.dc === color ? ' active' : '')}
                           style={`background: #${color}`}
                           onClick={() => setParam('dc', color)}/>
                    )}
                    <br/>
                    {/* Имя пользователя и пароль к Web интерфейсу */}
                    <p className="bold"><Message key="n.m.id.user"/></p>
                    <Input className="input w75 large" value={du} type="text" maxlength="16"
                           onInput={linkState('device.du')}/>
                    <p className="bold"><Message key="n.m.id.psk"/></p>
                    <Input className="input w75 large" value={dp} type="text" maxlength="16"
                           onInput={linkState('device.dp')}/>
                    <br/><br/>
                    <Button className="button w50 blue large" onClick={() => this.context.sendWS("setId")}><Message
                        key="a.save"/></Button>
                </div>
            </div>
        )
    }
}