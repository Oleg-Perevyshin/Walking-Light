// Страница "НАСТРОЙКИ" (левая панель)

import {h, Component} from "preact";
import Message from "../components/Message";

export default class Parameters extends Component {
    render() {
        return (
            <div style="padding: 0 1em;">
                <h2 className="tPan ltr"><Message key="n.l.t"/></h2>
                <p className={"indent justify"}><Message key="n.l.c"/></p>
                <p className={"indent justify"}><Message key="n.l.c1"/></p>
            </div>
        );
    }
}
