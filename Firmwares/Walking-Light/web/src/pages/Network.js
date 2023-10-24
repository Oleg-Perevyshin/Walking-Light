// Страница "НАСТРОЙКИ" (центр)

import {h, Component} from "preact";
import WiFiConfig from "../components/WiFiConfig";
import RFConfig from "../components/RFConfig";
import ActInfo from "../components/ActInfo";
import Message from "../components/Message";
import Button from "../components/Button";


export default class NetworkMain extends Component {


    render({}) {
        return (
            <div>
                <h2 className="tPan"><Message key="n.m.t"/></h2>
                <div>
                    <WiFiConfig/>
                    <RFConfig/>
                    <ActInfo/>
                </div>
                <Button className="button w75 red large" onClick={() => this.context.sendWS("rst")}><Message
                    key="a.rst"/></Button>
                <br/>
                <br/>
            </div>
        );
    }
}
