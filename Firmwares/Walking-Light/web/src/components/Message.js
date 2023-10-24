import {translate} from '../util'

const Message = ({key}, {dictionary}) => translate(dictionary, key);

export default Message;
