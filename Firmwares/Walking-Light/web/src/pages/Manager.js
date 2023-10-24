// Страница "СЕТЬ" (центр)

import {h, Component} from "preact";
import Message from "../components/Message";
import Button from "../components/Button";
import Loading from "../components/Loading";

const getUniqueValues = (value, data) => {
    const uniqueColors = [];
    for (let item in data) {
        if ((value in data[item]) && (uniqueColors.indexOf(data[item][value]) === -1)) uniqueColors.push(data[item][value]);
    }
    return uniqueColors;
};

export default class ManagerMain extends Component {

    constructor() {
        super();
        this.setState({
            loading: false
        });
        this.scan = this.scan.bind(this);
        this.getIcon = this.getIcon.bind(this);
        this.getDev = this.getDev.bind(this);
    }

    scan() {
        this.setState({loading: true});
        this.context.sendWS("devsScan");
        setTimeout(() => {
            this.setState({loading: false});
        }, 5000);
    }

    getIcon(ip) {
        fetch(`//${ip}/icon`)
            .then(response => {
                return response.text()
                    .then(content => {
                        document.getElementById(`icon_${ip}`).innerHTML = content;
                    });
            });
    }

    getDev(ip) {
        fetch(`//${ip}/dev`)
            .then(response => {
                return response.text()
                    .then(content => {
                        document.getElementById(`cont_${ip}`).innerHTML = content;
                    });
            });
    }

    render() {

        const {devices, colorSorting, setColorSorting, typeSorting, setTypeSorting, clearSorting} = this.context;

        return (
            <div>
                <h2 className="tPan"><Message key="m.m.t"/></h2>
                {this.state.loading ? <Loading/> : (
                    <div>
                        <div className="block-in">
                            <h2 className="bold"><Message key="m.m.select"/></h2>
                            <p><Message key="m.m.color"/></p>
                            {
                                getUniqueValues("dc", devices).map(color => (
                                    <a className="cDev" style={`background: #${color}`}
                                       onClick={() => setColorSorting(color)}/>)
                                )
                            }
                            <p><Message key="m.m.type"/></p>
                            {
                                getUniqueValues("dt", devices).map(type => (
                                    <Button className="button w30 gray large"
                                            onClick={() => setTypeSorting(type)}>{type}</Button>)
                                )
                            }
                            <Button className="button w75 blue large" onClick={clearSorting}><Message
                                key="a.alldevs"/></Button>
                            <Button className="button w75 red large" onClick={this.scan}><Message
                                key="a.scan"/></Button>
                            <br/><br/>
                        </div>
                        <div id="data">
                            {
                                Object.keys(devices)
                                    .filter(key => {
                                        let device = devices[key];
                                        if (colorSorting || typeSorting) {
                                            return colorSorting && device.dc === colorSorting || typeSorting && device.dt === typeSorting;
                                        }
                                        return true;
                                    })

                                    .map(device =>
                                        <div className="block" id={devices[device].ip}
                                             style={`border-top: .5em solid #${devices[device].dc}`}>
                                            {/* Заголовок устройства */}
                                            <div className="dev-wrp">
                                                <div className="dev-icon" id={`icon_${devices[device].ip}`}>
                                                    {this.getIcon(`${devices[device].ip}`)}
                                                </div>
                                                <div className="dev-title">
                                                    <a target="_blank" className="bold"
                                                       href={`//${devices[device].ip}/#control`}>{devices[device].dm}
                                                        | {devices[device].dn}</a>
                                                </div>
                                            </div>
                                            {/* Кнопка сварачивания расширенных данных */}
                                            <input type="checkbox" id={devices[device].da} className="checker"/>
                                            <label for={devices[device].da} className="toggle"/>
                                            <div id={`cont_${devices[device].ip}`} className="devhtm spl">
                                                {this.getDev(`${devices[device].ip}`)}
                                            </div>
                                            {/* Низ блока устройства */}
                                            <div className="dev-bottom">IP:{devices[device].ip}
                                                RA:{devices[device].da}</div>
                                        </div>
                                    )
                            }
                        </div>
                    </div>
                )}

            </div>
        );
    }
}
